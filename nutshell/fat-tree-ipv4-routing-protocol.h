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
 * fat-tree-ipv4-routing-protocol.h
 *
 *  Created on: Nov 24, 2015
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#ifndef FAT_TREE_IPV4_ROUTING_PROTOCOL_H
#define FAT_TREE_IPV4_ROUTING_PROTOCOL_H

#include <list>
#include <stdint.h>
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"
#include "ns3/ptr.h"
#include "ns3/ipv4.h"
#include "ns3/ipv4-routing-protocol.h"

#include "fat-tree-ipv4-rte.h"
#include "fat-tree-ipv4-routing-header.h"


namespace ns3 {


class Packet;
class NetDevice;
class Ipv4Interface;
class Ipv4Address;
class Ipv4Header;
class Ipv4RoutingTableEntry;
class Node;

class FatTreeIpv4RoutingProtocol: public Ipv4RoutingProtocol {
public:

	/**
	* \brief Get the type ID.
	* \return the object TypeId
	*/
	static TypeId GetTypeId (void);

	FatTreeIpv4RoutingProtocol();
	virtual ~FatTreeIpv4RoutingProtocol();

	// These methods inherited from base class
	virtual Ptr<Ipv4Route> RouteOutput (Ptr<Packet> p, const Ipv4Header &header, Ptr<NetDevice> oif, Socket::SocketErrno &sockerr);

	virtual bool RouteInput  (Ptr<const Packet> p, const Ipv4Header &header, Ptr<const NetDevice> idev,
							UnicastForwardCallback ucb, MulticastForwardCallback mcb,
							LocalDeliverCallback lcb, ErrorCallback ecb);
	virtual void NotifyInterfaceUp (uint32_t interface);
	virtual void NotifyInterfaceDown (uint32_t interface);
	virtual void NotifyAddAddress (uint32_t interface, Ipv4InterfaceAddress address);
	virtual void NotifyRemoveAddress (uint32_t interface, Ipv4InterfaceAddress address);
	virtual void SetIpv4 (Ptr<Ipv4> ipv4);
	virtual void PrintRoutingTable (Ptr<OutputStreamWrapper> stream) const;

	/**
	* \brief Add a network route to the global routing table.
	*
	* \param network The Ipv4Address network for this route.
	* \param networkMask The Ipv4Mask to extract the network.
	* \param nextHop The next hop in the route to the destination network.
	* \param interface The network interface index used to send packets to the
	* destination.
	*
	* \see Ipv4Address
	*/
	void AddNetworkRouteTo (Ipv4Address network,
						  Ipv4Mask networkMask,
						  Ipv4Address nextHop,
						  uint32_t interface);

	/**
	* \brief Add a network route to the global routing table.
	*
	* \param network The Ipv4Address network for this route.
	* \param networkMask The Ipv4Mask to extract the network.
	* \param interface The network interface index used to send packets to the
	* destination.
	*
	* \see Ipv4Address
	*/
	void AddNetworkRouteTo (Ipv4Address network,
						  Ipv4Mask networkMask,
						  uint32_t interface);

	/**
	* \brief Add a network route to the global routing table.
	*
	* \param network The Ipv4Address network for this route.
	* \param networkMask The Ipv4Mask to extract the network.
	* \param nextHop The next hop in the route to the destination network.
	* \param interface The network interface index used to send packets to the
	* destination.
	*
	* \see Ipv4Address
	*/
	void AddSuffixNetworkRouteTo (Ipv4Address network,
						  Ipv4Mask networkMask,
						  Ipv4Address nextHop,
						  uint32_t interface);

	/**
	* \brief Add a network route to the global routing table.
	*
	* \param network The Ipv4Address network for this route.
	* \param networkMask The Ipv4Mask to extract the network.
	* \param interface The network interface index used to send packets to the
	* destination.
	*
	* \see Ipv4Address
	*/
	void AddSuffixNetworkRouteTo (Ipv4Address network,
						  Ipv4Mask networkMask,
						  uint32_t interface);

	/**
	* \brief Get the number of individual unicast routes that have been added
	* to the routing table.
	*
	* \warning The default route counts as one of the routes.
	* \returns the number of routes
	*/
	uint32_t GetNRoutes (void) const;

	/**
	* \brief Get a route from the global unicast routing table.
	*
	* Externally, the unicast global routing table appears simply as a table with
	* n entries.  The one subtlety of note is that if a default route has been set
	* it will appear as the zeroth entry in the table.  This means that if you
	* add only a default route, the table will have one entry that can be accessed
	* either by explicitly calling GetDefaultRoute () or by calling GetRoute (0).
	*
	* Similarly, if the default route has been set, calling RemoveRoute (0) will
	* remove the default route.
	*
	* \param i The index (into the routing table) of the route to retrieve.  If
	* the default route has been set, it will occupy index zero.
	* \return If route is set, a pointer to that Ipv4RoutingTableEntry is returned, otherwise
	* a zero pointer is returned.
	*
	* \see Ipv4RoutingTableEntry
	* \see Ipv4GlobalRouting::RemoveRoute
	*/
	Ipv4RoutingTableEntry *GetRoute (uint32_t i) const;

protected:
	void DoDispose (void);

	/**
	* Start protocol operation
	*/
	void DoInitialize ();
private:

	/// container of Ipv4RoutingTableEntry (routes to networks)
	typedef std::list<Ipv4RoutingTableEntry *> NetworkRoutes;
	/// const iterator of container of Ipv4RoutingTableEntry (routes to networks)
	typedef std::list<Ipv4RoutingTableEntry *>::const_iterator NetworkRoutesCI;
	/// iterator of container of Ipv4RoutingTableEntry (routes to networks)
	typedef std::list<Ipv4RoutingTableEntry *>::iterator NetworkRoutesI;

	/// container of Ipv4RoutingTableEntry (routes to networks)
	typedef std::list<Ipv4RoutingTableEntry *> SuffixRoutes;
	/// const iterator of container of Ipv4RoutingTableEntry (routes to networks)
	typedef std::list<Ipv4RoutingTableEntry *>::const_iterator SuffixRoutesCI;
	/// iterator of container of Ipv4RoutingTableEntry (routes to networks)
	typedef std::list<Ipv4RoutingTableEntry *>::iterator SuffixRoutesI;

	/// Socket list type
	typedef std::map< Ptr<Socket>, uint32_t> SocketList;
	/// Socket list type iterator
	typedef std::map<Ptr<Socket>, uint32_t>::iterator SocketListI;
	/// Socket list type const iterator
	typedef std::map<Ptr<Socket>, uint32_t>::const_iterator SocketListCI;

	Ptr<Ipv4Route> LookupGlobal (Ipv4Address dest, Ptr<NetDevice> oif = 0);

	NetworkRoutes m_networkRoutes;       //!< Routes to networks
	SuffixRoutes m_suffixRoutes;		//!<Routes to networks

	Ptr<Ipv4> m_ipv4; //!< associated IPv4 instance

	SocketList m_sendSocketList; //!< list of sockets for sending (socket, interface index)
	Ptr<Socket> m_recvSocket; //!< receive socket
	std::vector<Ipv4Address> m_ifbroadcast;

	bool m_addedDefaultGw;
	bool m_isCoreSw;
	Ipv4Address m_switch;

	const static uint32_t FAT_ROUTING_PORT = 3287;


	void Receive (Ptr<Socket> socket);

	void DoSendUpdate();

	void CalculateRoutingTable(FatTreeIpv4RoutingHeader & hdr, uint32_t receivingIf, Ipv4Address senderAddr);

	void FindSwitchIndex();

	Ipv4RoutingTableEntry* FindCloseMatch(std::vector<Ipv4RoutingTableEntry*> allRoutes, Ipv4Address dest);

	Ipv4Address CorePodAddress(uint32_t p);
	Ipv4Address SwitchPod(Ipv4Address & addr);
	Ipv4Address SwitchIndexAddress(Ipv4Address & addr);
	std::string PrefixLenToString(uint16_t m);
};

} /* namespace ns3 */

#endif /* SCRATCH_FATTREEROUTINGTEST_FAT_TREE_IPV4_ROUTING_PROTOCOL_H_ */
