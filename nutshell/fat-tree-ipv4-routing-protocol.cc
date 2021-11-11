/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 * fat-tree-ipv4-routing-protocol.cc
 *
 *  Created on: Nov 24, 2015
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include "ns3/names.h"
#include "ns3/log.h"
#include "ns3/abort.h"
#include "ns3/assert.h"
#include "ns3/unused.h"
#include "ns3/simulator.h"
#include "ns3/object.h"
#include "ns3/packet.h"
#include "ns3/names.h"
#include "ns3/net-device.h"
#include "ns3/ipv4-route.h"
#include "ns3/ipv4-routing-table-entry.h"
#include "ns3/boolean.h"
#include "ns3/node.h"
#include "ns3/ipv4-packet-info-tag.h"
#include "ns3/udp-header.h"

#include "fat-tree-ipv4-routing-protocol.h"
#include "fat-tree-ipv4-rte.h"
#include "fat-tree-ipv4-routing-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("FatTreeIpv4RoutingProtocol");

NS_OBJECT_ENSURE_REGISTERED (FatTreeIpv4RoutingProtocol);

TypeId
FatTreeIpv4RoutingProtocol::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::FatTreeIpv4RoutingProtocol")
    .SetParent<Ipv4RoutingProtocol> ()
    .AddConstructor<FatTreeIpv4RoutingProtocol>()
    ;
  return tid;
}

FatTreeIpv4RoutingProtocol::FatTreeIpv4RoutingProtocol()
{
	NS_LOG_FUNCTION(this);
}

FatTreeIpv4RoutingProtocol::~FatTreeIpv4RoutingProtocol()
{
}

void
FatTreeIpv4RoutingProtocol::DoInitialize()
{
	NS_LOG_FUNCTION(this);
	NS_LOG_INFO("\n\n---------\n About to create routing table \n---------\n\n");
//	CalculateRoutingTable();
	m_addedDefaultGw = false;
	m_isCoreSw = false;

	FindSwitchIndex();

	// looping through all ipv4 interfaces of node
	for(uint32_t i = 0; i < m_ipv4->GetNInterfaces(); i++)
	{
		for(uint32_t j = 0; j < m_ipv4->GetNAddresses(i); j++)
		{
			Ipv4InterfaceAddress address = m_ipv4->GetAddress(i,j);
			m_ifbroadcast.push_back(address.GetBroadcast());
			//NS_LOG_INFO("Address: " << address << "\n" << "Scope :" << address.GetScope());
			if(address.GetScope() == Ipv4InterfaceAddress::GLOBAL)
			{
				TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
				Ptr<Node> theNode = GetObject<Node> ();
				Ptr<Socket> socket = Socket::CreateSocket (theNode, tid);
				InetSocketAddress local = InetSocketAddress (address.GetLocal(), FAT_ROUTING_PORT);
				int ret = socket->Bind (local);
				NS_ASSERT_MSG (ret == 0, "Bind unsuccessful");
				socket->BindToNetDevice (m_ipv4->GetNetDevice (i));
				socket->ShutdownRecv ();
				m_sendSocketList[socket] = i;
			}

		}

	}

	if(!m_recvSocket)
	{
		NS_LOG_LOGIC ("FatTreeRouting: adding receiving socket");
		TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
		Ptr<Node> theNode = GetObject<Node> ();
		m_recvSocket = Socket::CreateSocket (theNode, tid);
		InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), FAT_ROUTING_PORT);
		m_recvSocket->Bind (local);
		m_recvSocket->SetRecvCallback (MakeCallback (&FatTreeIpv4RoutingProtocol::Receive, this));
		m_recvSocket->SetRecvPktInfo (true);
	}

	// begining the transmission
	DoSendUpdate();
	Ipv4RoutingProtocol::DoInitialize();
}


void
FatTreeIpv4RoutingProtocol::AddNetworkRouteTo (Ipv4Address network,
												  Ipv4Mask networkMask,
												  Ipv4Address nextHop,
												  uint32_t interface)
{
  NS_LOG_FUNCTION (this << network << networkMask << nextHop << interface);
  Ipv4RoutingTableEntry *route = new Ipv4RoutingTableEntry ();
  *route = Ipv4RoutingTableEntry::CreateNetworkRouteTo (network,
                                                        networkMask,
                                                        nextHop,
                                                        interface);
  m_networkRoutes.push_back (route);
}

void
FatTreeIpv4RoutingProtocol::AddNetworkRouteTo (Ipv4Address network,
												  Ipv4Mask networkMask,
												  uint32_t interface)
{
  NS_LOG_FUNCTION (this << network << networkMask << interface);
  Ipv4RoutingTableEntry *route = new Ipv4RoutingTableEntry ();
  *route = Ipv4RoutingTableEntry::CreateNetworkRouteTo (network,
                                                        networkMask,
                                                        interface);
  m_networkRoutes.push_back (route);
}

void
FatTreeIpv4RoutingProtocol::AddSuffixNetworkRouteTo (Ipv4Address network,
												  Ipv4Mask networkMask,
												  Ipv4Address nextHop,
												  uint32_t interface)
{
  NS_LOG_FUNCTION (this << network << networkMask << nextHop << interface);
  Ipv4RoutingTableEntry *route = new Ipv4RoutingTableEntry ();
  *route = Ipv4RoutingTableEntry::CreateNetworkRouteTo (network,
                                                        networkMask,
                                                        nextHop,
                                                        interface);
  m_suffixRoutes.push_back (route);
}

void
FatTreeIpv4RoutingProtocol::AddSuffixNetworkRouteTo (Ipv4Address network,
												  Ipv4Mask networkMask,
												  uint32_t interface)
{
  NS_LOG_FUNCTION (this << network << networkMask << interface);
  Ipv4RoutingTableEntry *route = new Ipv4RoutingTableEntry ();
  *route = Ipv4RoutingTableEntry::CreateNetworkRouteTo (network,
                                                        networkMask,
                                                        interface);
  m_suffixRoutes.push_back (route);
}

Ptr<Ipv4Route>
FatTreeIpv4RoutingProtocol::LookupGlobal (Ipv4Address dest, Ptr<NetDevice> oif)
{

  NS_LOG_FUNCTION (this << dest << oif);
  NS_LOG_LOGIC ("Looking for route for destination " << dest);
  Ptr<Ipv4Route> rtentry = 0;
  // store all available routes
  typedef std::vector<Ipv4RoutingTableEntry*> RouteVec_t;
  RouteVec_t allRoutes;

  if(m_isCoreSw)
  {
	  NS_LOG_INFO("its a core switch");
	  for (NetworkRoutesI i = m_networkRoutes.begin();
	  				i != m_networkRoutes.end(); i++)
		{
			Ipv4Mask mask("255.255.0.0");
			Ipv4Address entry = (*i)->GetDestNetwork();
			if (mask.IsMatch(dest, entry))
			{
				if (oif != 0)
				{
					if (oif != m_ipv4->GetNetDevice((*i)->GetInterface()))
					{
						NS_LOG_LOGIC("Not on requested interface, skipping");
						continue;
					}
				}
				// found a route
				allRoutes.push_back(*i);
			}
		}
  }
  else
  {
	  NS_LOG_INFO("No Its not a core switch");
		for (NetworkRoutesI i = m_networkRoutes.begin();
				i != m_networkRoutes.end(); i++)
		{
			Ipv4Mask mask = (*i)->GetDestNetworkMask();
			Ipv4Address entry = (*i)->GetDestNetwork();
			if (mask.IsMatch(dest, entry))
			{
				if (oif != 0)
				{
					if (oif != m_ipv4->GetNetDevice((*i)->GetInterface()))
					{
						NS_LOG_LOGIC("Not on requested interface, skipping");
						continue;
					}
				}
				// found a route
				allRoutes.push_back(*i);
			}
		}

		if (allRoutes.size() == 0)
		{
			NS_LOG_INFO("checking the suffix, suffix size: " << m_suffixRoutes.size());
			for (SuffixRoutesI i = m_suffixRoutes.begin();
					i != m_suffixRoutes.end(); i++)
			{
				allRoutes.push_back(*i);
			}
		}
	}

  Ipv4RoutingTableEntry* route = 0;
  if(allRoutes.size() > 0)
  {
	  if(allRoutes.size() > 1)
	  {
		  route = FindCloseMatch(allRoutes, dest);
	  }
	  else
	  {
		  route = allRoutes.at(0);
	  }

	rtentry = Create<Ipv4Route>();

	rtentry->SetDestination (route->GetDest());


	rtentry->SetSource (m_ipv4->GetAddress (route->GetInterface (), 0).GetLocal ());
	rtentry->SetGateway (route->GetGateway ());
	uint32_t interfaceIdx = route->GetInterface ();
	rtentry->SetOutputDevice (m_ipv4->GetNetDevice (interfaceIdx));
	NS_LOG_INFO("---------------------------------");
	NS_LOG_INFO("At Node: "<< this->GetObject<Node> ()->GetId() <<"\nForwarding packet for: \t" << dest
				  	  	  << "\nTo next hop with dest\t" << route->GetDest()
						  << "\n Destination Network " << route->GetDestNetwork()
						  <<"\nOuput Port: \t\t" << route->GetInterface());
	NS_LOG_INFO("---------------------------------");
	NS_LOG_INFO("From lookup " << rtentry);
	return rtentry;
  }
  else
  {
	  return 0;
  }
}

uint32_t
FatTreeIpv4RoutingProtocol::GetNRoutes (void) const
{
  NS_LOG_FUNCTION (this);
  uint32_t n = 0;
  n += m_networkRoutes.size ();
  n += m_suffixRoutes.size ();
  return n;
}

Ipv4RoutingTableEntry *
FatTreeIpv4RoutingProtocol::GetRoute (uint32_t index) const
{
  NS_LOG_FUNCTION (this << index);
  uint32_t tmp = 0;
	if (index < m_networkRoutes.size ())
	{
		for (NetworkRoutesCI j = m_networkRoutes.begin ();
			 j != m_networkRoutes.end ();
			 j++)
		{
			if (tmp == index)
			{
				return *j;
			}
			tmp++;
		}
	}

	index -= m_networkRoutes.size ();
	tmp = 0;
  if (index < m_suffixRoutes.size ())
    {
      for (SuffixRoutesCI i = m_suffixRoutes.begin ();
           i != m_suffixRoutes.end ();
           i++)
        {
          if (tmp  == index)
            {
              return *i;
            }
          tmp++;
        }
    }


  NS_ASSERT (false);
  // quiet compiler.
  return 0;
}

void
FatTreeIpv4RoutingProtocol::DoDispose (void)
{
  NS_LOG_FUNCTION (this);
  for (NetworkRoutesI j = m_networkRoutes.begin ();
       j != m_networkRoutes.end ();
       j = m_networkRoutes.erase (j))
    {
      delete (*j);
    }
  for (SuffixRoutesI j = m_suffixRoutes.begin ();
         j != m_suffixRoutes.end ();
         j = m_suffixRoutes.erase (j))
  {
	delete (*j);
  }

  Ipv4RoutingProtocol::DoDispose ();
}

// Formatted like output of "route -n" command
void
FatTreeIpv4RoutingProtocol::PrintRoutingTable (Ptr<OutputStreamWrapper> stream) const
{
  NS_LOG_FUNCTION (this << stream);
  std::ostream* os = stream->GetStream ();
  if (GetNRoutes () > 0)
    {
      *os << "Destination     Gateway         Genmask         Flags Metric Ref    Use Iface" << std::endl;
      for (uint32_t j = 0; j < GetNRoutes (); j++)
        {
          std::ostringstream dest, gw, mask, flags;
          Ipv4RoutingTableEntry route = GetRoute (j);
          dest << route.GetDest ();
          *os << std::setiosflags (std::ios::left) << std::setw (16) << dest.str ();
          gw << route.GetGateway ();
          *os << std::setiosflags (std::ios::left) << std::setw (16) << gw.str ();
          mask << route.GetDestNetworkMask ();
          *os << std::setiosflags (std::ios::left) << std::setw (16) << mask.str ();
          flags << "U";
          if (route.IsHost ())
            {
              flags << "H";
            }
          else if (route.IsGateway ())
            {
              flags << "G";
            }
          *os << std::setiosflags (std::ios::left) << std::setw (6) << flags.str ();
          // Metric not implemented
          *os << "-" << "      ";
          // Ref ct not implemented
          *os << "-" << "      ";
          // Use not implemented
          *os << "-" << "   ";
          if (Names::FindName (m_ipv4->GetNetDevice (route.GetInterface ())) != "")
            {
              *os << Names::FindName (m_ipv4->GetNetDevice (route.GetInterface ()));
            }
          else
            {
              *os << route.GetInterface ();
            }
          *os << std::endl;
        }
    }
}

Ptr<Ipv4Route>
FatTreeIpv4RoutingProtocol::RouteOutput (Ptr<Packet> p, const Ipv4Header &header, Ptr<NetDevice> oif, Socket::SocketErrno &sockerr)
{
  NS_LOG_FUNCTION (this << p << &header << oif << &sockerr);
  NS_LOG_INFO("RoutOutput");
//
// First, see if this is a multicast packet we have a route for.  If we
// have a route, then send the packet down each of the specified interfaces.
//
  if (header.GetDestination ().IsMulticast ())
    {
      NS_LOG_LOGIC ("Multicast destination-- returning false");
      return 0; // Let other routing protocols try to handle this
    }
//
// See if this is a unicast packet we have a route for.
//
  NS_LOG_LOGIC ("Unicast destination- looking up");
  Ptr<Ipv4Route> rtentry = LookupGlobal (header.GetDestination (), oif);
  if (rtentry)
    {
      sockerr = Socket::ERROR_NOTERROR;
//      NS_LOG_INFO("")
    }
  else
    {
      sockerr = Socket::ERROR_NOROUTETOHOST;
    }
  return rtentry;
}

bool
FatTreeIpv4RoutingProtocol::RouteInput  (Ptr<const Packet> p, const Ipv4Header &header, Ptr<const NetDevice> idev,
											UnicastForwardCallback ucb, MulticastForwardCallback mcb,
											LocalDeliverCallback lcb, ErrorCallback ecb)
{

  NS_LOG_FUNCTION (this << p << header << header.GetSource () << header.GetDestination () << idev << &lcb << &ecb);
  NS_LOG_INFO("RouteInput: Node " << this->GetObject<Node>()->GetId());
  // Check if input device supports IP
  NS_ASSERT (m_ipv4->GetInterfaceForDevice (idev) >= 0);
  uint32_t iif = m_ipv4->GetInterfaceForDevice (idev);

  if (header.GetDestination ().IsMulticast ())
    {
      NS_LOG_LOGIC ("Multicast destination-- returning false");
      return false; // Let other routing protocols try to handle this
    }

  if (header.GetDestination ().IsBroadcast ())
    {
      NS_LOG_LOGIC ("For me (Ipv4Addr broadcast address)");
      /// \todo  Local Deliver for broadcast
      /// \todo  Forward broadcast
    }

  /// \todo  Configurable option to enable \RFC{1222} Strong End System Model
  // Right now, we will be permissive and allow a source to send us
  // a packet to one of our other interface addresses; that is, the
  // destination unicast address does not match one of the iif addresses,
  // but we check our other interfaces.  This could be an option
  // (to remove the outer loop immediately below and just check iif).
  for (uint32_t j = 0; j < m_ipv4->GetNInterfaces (); j++)
    {
      for (uint32_t i = 0; i < m_ipv4->GetNAddresses (j); i++)
        {
          Ipv4InterfaceAddress iaddr = m_ipv4->GetAddress (j, i);
          Ipv4Address addr = iaddr.GetLocal ();
          if (addr.IsEqual (header.GetDestination ()))
            {
              if (j == iif)
                {
                  NS_LOG_LOGIC ("For me (destination " << addr << " match)");
                }
              else
                {
                  NS_LOG_LOGIC ("For me (destination " << addr << " match) on another interface " << header.GetDestination ());
                }
              lcb (p, header, iif);
              return true;
            }
          if (header.GetDestination ().IsEqual (iaddr.GetBroadcast ()))
            {
              NS_LOG_LOGIC ("For me (interface broadcast address)");
              lcb (p, header, iif);
              return true;
            }
          NS_LOG_LOGIC ("Address "<< addr << " not a match");
        }
    }
  // Check if input device supports IP forwarding
  if (m_ipv4->IsForwarding (iif) == false)
    {
      NS_LOG_LOGIC ("Forwarding disabled for this interface");
      ecb (p, header, Socket::ERROR_NOROUTETOHOST);
      return false;
    }
  // Next, try to find a route
  NS_LOG_LOGIC ("Unicast destination- looking up global route");
  Ptr<Ipv4Route> rtentry = LookupGlobal (header.GetDestination ());
  if (rtentry != 0)
    {
      NS_LOG_LOGIC ("Found unicast destination- calling unicast callback");
      ucb (rtentry, p, header);
      return true;
    }
  else
    {
      NS_LOG_LOGIC ("Did not find unicast destination- returning false");
      return false; // Let other routing protocols try to handle this
                    // route request.
    }
}
void
FatTreeIpv4RoutingProtocol::NotifyInterfaceUp (uint32_t i)
{
  NS_LOG_FUNCTION (this << i);

}

void
FatTreeIpv4RoutingProtocol::NotifyInterfaceDown (uint32_t i)
{
  NS_LOG_FUNCTION (this << i);

}

void
FatTreeIpv4RoutingProtocol::NotifyAddAddress (uint32_t interface, Ipv4InterfaceAddress address)
{
  NS_LOG_FUNCTION (this << interface << address);

}

void
FatTreeIpv4RoutingProtocol::NotifyRemoveAddress (uint32_t interface, Ipv4InterfaceAddress address)
{
  NS_LOG_FUNCTION (this << interface << address);

}

void
FatTreeIpv4RoutingProtocol::SetIpv4 (Ptr<Ipv4> ipv4)
{
  NS_LOG_FUNCTION (this << ipv4);
  NS_ASSERT (m_ipv4 == 0 && ipv4 != 0);
  m_ipv4 = ipv4;
}

void
FatTreeIpv4RoutingProtocol::Receive (Ptr<Socket> socket)
{
	NS_LOG_FUNCTION(this);
	Ptr<Packet> packet = socket->Recv();
	Ipv4PacketInfoTag ifaceInfo;

	if(!packet->RemovePacketTag(ifaceInfo))
	{
		NS_ABORT_MSG ("No incoming interface on FatTreeRouteUpdate message, aborting.");
	}
	uint32_t incomingIf = ifaceInfo.GetRecvIf();
	Ptr<Node> node = this->GetObject<Node> ();
	Ptr<NetDevice> dev = node->GetDevice (incomingIf);
	uint32_t ipInterfaceIndex = m_ipv4->GetInterfaceForDevice (dev);
	SocketAddressTag tag;
	if (!packet->RemovePacketTag (tag))
	{
	  NS_ABORT_MSG ("No incoming sender address on FatTreeRouteUpdate message, aborting.");
	}
	Ipv4Address senderAddress = InetSocketAddress::ConvertFrom(tag.GetAddress()).GetIpv4();
	FatTreeIpv4RoutingHeader hdr;

	packet->RemoveHeader(hdr);
	CalculateRoutingTable(hdr, ipInterfaceIndex, senderAddress);
}

void
FatTreeIpv4RoutingProtocol::DoSendUpdate()
{
	NS_LOG_FUNCTION(this);
	FatTreeIpv4RoutingHeader hdr;


	for(uint32_t i = 0; i < m_ipv4->GetNInterfaces(); i++)
	{
		for(uint32_t j = 0; j < m_ipv4->GetNAddresses(i); j++)
		{
			FatTreeIpv4Rte rte;
			Ipv4InterfaceAddress address = m_ipv4->GetAddress(i, j);
			Ipv4Address workingAdd = address.GetLocal();
//			Ipv4Address swIndex = SwitchIndexAddress(workingAdd);
//			if( (swIndex < m_switch || swIndex.IsEqual(m_switch)) &&
//					(!workingAdd.IsEqual("127.0.0.1")) )
//			{
				Ipv4Mask workingAddMask = address.GetMask();
				Ipv4Address netPart = workingAdd.CombineMask(workingAddMask);
	//			NS_LOG_INFO("Interface: "<< i
	//						<<"\nAddress:\t"<< workingAdd
	//						<<"\nNetwork:\t" << netPart
	//						<< "\nBroadcast: \t" << address.GetBroadcast()
	//						<<"\nPrefix L: \t" << workingAddMask.GetPrefixLength());
				rte.SetPrefix(netPart);
				rte.SetPrefixLen(workingAddMask.GetPrefixLength());
				hdr.AddRte(rte);
//			}

		}
	}
	uint32_t bcounter = 0;
	for (SocketListI iter = m_sendSocketList.begin (); iter != m_sendSocketList.end (); iter++ )
	{
		Ptr<Packet> p = Create<Packet>();
//		uint32_t interface = iter->second;

		p->AddHeader(hdr);
		if(bcounter != m_ifbroadcast.size())
		{
			iter->first->SendTo(p,0,
					InetSocketAddress(m_ifbroadcast[bcounter], FAT_ROUTING_PORT));
			bcounter++;
		}
	}
}

void
FatTreeIpv4RoutingProtocol::CalculateRoutingTable(FatTreeIpv4RoutingHeader & hdr, uint32_t receivingIf, Ipv4Address senderAddr)
{
	NS_LOG_FUNCTION(this);

	if(m_ipv4->GetNInterfaces() == 2)
	{
		if((!m_addedDefaultGw) && !senderAddr.IsEqual(Ipv4Address("127.0.0.1")))
		{

			AddNetworkRouteTo(Ipv4Address("0.0.0.0"), Ipv4Mask("0.0.0.0"), senderAddr, receivingIf);
			m_addedDefaultGw = true;
		}
		return;
	}
	uint32_t tIface = m_ipv4->GetNInterfaces() - 1;
	Ipv4Address corePod = CorePodAddress(tIface);

	std::list<FatTreeIpv4Rte> rtes = hdr.GetRteList();/*
  // store all available routes that bring packets to their destination
  typedef std::vector<Ipv4RoutingTableEntry*> RouteVec_t;
  RouteVec_t allRoutes;

  NS_LOG_LOGIC ("Number of m_suffixRoutes = " << m_suffixRoutes.size ());
  for (SuffixRoutesCI i = m_suffixRoutes.begin ();
       i != m_suffixRoutes.end ();
       i++)
    {
      NS_ASSERT ((*i)->IsHost ());
      if ((*i)->GetDest ().IsEqual (dest))
        {
          if (oif != 0)
            {
              if (oif != m_ipv4->GetNetDevice ((*i)->GetInterface ()))
                {
                  NS_LOG_LOGIC ("Not on requested interface, skipping");
                  continue;
                }
            }
          allRoutes.push_back (*i);
          NS_LOG_LOGIC (allRoutes.size () << "Found global host route" << *i);
        }
    }
  if (allRoutes.size () == 0) // if no host route is found
    {
      NS_LOG_LOGIC ("Number of m_networkRoutes" << m_networkRoutes.size ());
      for (NetworkRoutesI j = m_networkRoutes.begin ();
           j != m_networkRoutes.end ();
           j++)
        {
          Ipv4Mask mask = (*j)->GetDestNetworkMask ();
          Ipv4Address entry = (*j)->GetDestNetwork ();
          if (mask.IsMatch (dest, entry))
            {
              if (oif != 0)
                {
                  if (oif != m_ipv4->GetNetDevice ((*j)->GetInterface ()))
                    {
                      NS_LOG_LOGIC ("Not on requested interface, skipping");
                      continue;
                    }
                }
              allRoutes.push_back (*j);
              NS_LOG_LOGIC (allRoutes.size () << "Found global network route" << *j);
            }
        }
    }
  if (allRoutes.size () == 0)  // consider external if no host/network found
    {
      for (ASExternalRoutesI k = m_ASexternalRoutes.begin ();
           k != m_ASexternalRoutes.end ();
           k++)
        {
          Ipv4Mask mask = (*k)->GetDestNetworkMask ();
          Ipv4Address entry = (*k)->GetDestNetwork ();
          if (mask.IsMatch (dest, entry))
            {
              NS_LOG_LOGIC ("Found external route" << *k);
              if (oif != 0)
                {
                  if (oif != m_ipv4->GetNetDevice ((*k)->GetInterface ()))
                    {
                      NS_LOG_LOGIC ("Not on requested interface, skipping");
                      continue;
                    }
                }
              allRoutes.push_back (*k);
              break;
            }
        }
    }
  if (allRoutes.size () > 0 ) // if route(s) is found
    {
      // pick up one of the routes uniformly at random if random
      // ECMP routing is enabled, or always select the first route
      // consistently if random ECMP routing is disabled
      uint32_t selectIndex;
      if (m_randomEcmpRouting)
        {
          selectIndex = m_rand->GetInteger (0, allRoutes.size ()-1);
        }
      else
        {
          selectIndex = 0;
        }
      Ipv4RoutingTableEntry* route = allRoutes.at (selectIndex);
      // create a Ipv4Route object from the selected routing table entry
      rtentry = Create<Ipv4Route> ();
      rtentry->SetDestination (route->GetDest ());
      /// \todo handle multi-address case
      rtentry->SetSource (m_ipv4->GetAddress (route->GetInterface (), 0).GetLocal ());
      rtentry->SetGateway (route->GetGateway ());
      uint32_t interfaceIdx = route->GetInterface ();
      rtentry->SetOutputDevice (m_ipv4->GetNetDevice (interfaceIdx));
      return rtentry;
    }
  else
    {
      return 0;
    }
    */
//	return 0;

	for(std::list<FatTreeIpv4Rte>::iterator i = rtes.begin();
			i != rtes.end(); i++)
	{
		Ipv4Address workingAdr = i->GetPrefix();

		Ipv4Address swPodAdr = SwitchPod(workingAdr);

		// pod switches and nodes
		if(!corePod.IsEqual(swPodAdr) &&
				!workingAdr.IsEqual("127.0.0.0") &&
				!senderAddr.IsEqual(Ipv4Address("127.0.0.1")))
		{
			if(m_isCoreSw)
			{
				Ipv4Address podAddr = workingAdr.CombineMask("255.255.0.0");
				bool exist = false;
				for(NetworkRoutesI i = m_networkRoutes.begin();
						i != m_networkRoutes.end(); i++)
				{
					Ipv4Address tableAdd = (*i)->GetDestNetwork();

					if(tableAdd.IsEqual(podAddr))
					{
						exist = true;
					}

				}
				if(!exist)
				{
					AddNetworkRouteTo(podAddr, "255.255.0.0", senderAddr, receivingIf);
				}

			}
			else
			{
				Ipv4Address swIndex = SwitchIndexAddress(workingAdr);
				Ipv4Mask msk(PrefixLenToString(i->GetPrefixLen()).c_str());
				if(swIndex < m_switch || swIndex.IsEqual(m_switch))
				{

					AddNetworkRouteTo(workingAdr, msk, senderAddr, receivingIf);
				}
				else
				{
					Ipv4Address senderNet = senderAddr.CombineMask("255.255.255.252");
					if(senderNet.IsEqual(workingAdr))
					{
						AddSuffixNetworkRouteTo(workingAdr, msk, senderAddr, receivingIf);
//						NS_LOG_INFO("Address: " << workingAdr <<" int value" << workingAdr.Get());
					}
				}
//				NS_LOG_INFO("swIndex: " << swIndex <<" int value" << swIndex.Get());
			}
		}
		else if(corePod.IsEqual(swPodAdr) &&
				!workingAdr.IsEqual("127.0.0.0") &&
				!senderAddr.IsEqual(Ipv4Address("127.0.0.1")))
		{
			if(!m_isCoreSw)
			{
				Ipv4Address senderNet = senderAddr.CombineMask("255.255.255.252");
				Ipv4Mask msk(PrefixLenToString(i->GetPrefixLen()).c_str());
				if(senderNet.IsEqual(workingAdr))
				{
					AddSuffixNetworkRouteTo(workingAdr, msk, senderAddr, receivingIf);
				}
			}
		}

	}

	// now for suffix


}

void
FatTreeIpv4RoutingProtocol::FindSwitchIndex()
{
	uint32_t nIface = m_ipv4->GetNInterfaces() - 1;

//	NS_LOG_INFO("NIfaces: "<< nIface <<"Size of tmp: " << tmp.size());
	std::map<Ipv4Address, uint32_t> count;
	uint32_t coreCount = 0;
	if(nIface > 2)
	{
//		NS_LOG_INFO("**************************");
//		NS_LOG_INFO("Node No. \t" << GetObject<Node>()->GetId()
//					<<"\nNumber of interfaces: \t" << nIface
//					<<"\nnIface/2: \t"<< nIface/2);
		Ipv4Address coreAdd = CorePodAddress(nIface);
//		NS_LOG_INFO("---------------------------");
//		NS_LOG_INFO("Core POD Address: " << coreAdd);
//		NS_LOG_INFO("---------------------------");

		for(uint32_t i = 0; i < m_ipv4->GetNInterfaces(); i++)
		{
			for(uint32_t j = 0 ; j < m_ipv4->GetNAddresses(i); j++)
			{
//				NS_LOG_INFO("value of i: " << i << " | value of j: "<<j);
				Ipv4InterfaceAddress address = m_ipv4->GetAddress(i, j);
				Ipv4Address workingAddr = address.GetLocal();
//				NS_LOG_INFO("Working Addr: \t" << workingAddr);
				Ipv4Address podAddr = SwitchPod(workingAddr);
//				NS_LOG_INFO("POD Addr: \t" << podAddr);
				if(!coreAdd.IsEqual(podAddr) &&
						!workingAddr.IsEqual("127.0.0.1"))
				{
					Ipv4Address swIndexAddr = SwitchIndexAddress(workingAddr);
//					NS_LOG_INFO("Sw Index Addr: \t" << swIndexAddr);

					if(count.find(swIndexAddr) == count.end())
					{
						count[swIndexAddr] = 1;
					}
					else
					{
						count[swIndexAddr] = count[swIndexAddr] + 1;
					}
				} // end of condition to get switch
				else if(coreAdd.IsEqual(podAddr) &&
						!workingAddr.IsEqual("127.0.0.1"))
				{
					coreCount++;
				}
//				NS_LOG_INFO("---------------------------");
			} // end of address loop
		} // end of interface loop

//		NS_LOG_INFO("**************************");
	} // end of if
	if(!count.empty())
	{
		uint32_t max = 0;
		for(std::map<Ipv4Address, uint32_t>::iterator i = count.begin();
				i != count.end(); i++)
		{
	//		NS_LOG_INFO("Count of " << i->first << " is " << i->second);
			if(i->second > max)
			{
				max = i->second;
				m_switch = i->first;
			}

		}
		NS_LOG_INFO("Switch: " << m_switch);
	}
	if(coreCount > 2 && coreCount == nIface)
	{
		m_isCoreSw = true;
	}
}

Ipv4RoutingTableEntry*
FatTreeIpv4RoutingProtocol::FindCloseMatch(std::vector<Ipv4RoutingTableEntry*> allRoutes, Ipv4Address dest)
{
	int32_t diff = INT32_MAX;
	uint32_t destNo = dest.Get();
	int index = -1;
	for(uint32_t i = 0; i < allRoutes.size(); i++)
	{
		uint32_t rno = allRoutes.at(i)->GetDestNetwork().Get();
		int32_t tmpDiff = destNo - rno;
//		NS_LOG_INFO("Route: " << i << "\n" << "dest: " << allRoutes.at(i)->GetDestNetwork());
//		NS_LOG_INFO("tmpDiff" << tmpDiff);
		if(tmpDiff < 0)
		{
			tmpDiff = (-1) * tmpDiff;
		}

		if(tmpDiff < diff)
		{
			diff = tmpDiff;
			index = i;
		}
	}
//	NS_LOG_INFO("Destination Network: " << allRoutes.at(index)->GetDestNetwork() << " Destination Address" << allRoutes.at(index)->GetGateway());
	return allRoutes.at(index);
}

Ipv4Address
FatTreeIpv4RoutingProtocol::CorePodAddress(uint32_t p)
{
	std::ostringstream oss;
	oss << "0." << p << ".0.0";
	Ipv4Address corePodAdd(oss.str().c_str());
	Ipv4Mask corePodMask ("0.255.0.0");
	return corePodAdd.CombineMask(corePodMask);
}

Ipv4Address
FatTreeIpv4RoutingProtocol::SwitchPod(Ipv4Address & addr)
{
	Ipv4Mask msk("0.255.0.0");
	return addr.CombineMask(msk);
}

Ipv4Address
FatTreeIpv4RoutingProtocol::SwitchIndexAddress(Ipv4Address & addr)
{
	Ipv4Mask msk("0.0.255.0");
	return addr.CombineMask(msk);
}

std::string
FatTreeIpv4RoutingProtocol::PrefixLenToString(uint16_t m)
{
	std::ostringstream oss;
	oss <<"/"<< m;
	return oss.str();
}

} /* namespace ns3 */
