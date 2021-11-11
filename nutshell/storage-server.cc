/*
 * storage-server.cc
 *
 *  Created on: Aug 17, 2015
 *      Author: ubaid
 */
#include "storage-server.h"

#include "ns3/address.h"
#include "ns3/address-utils.h"
#include "ns3/log.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/node.h"
#include "ns3/socket.h"
#include "ns3/udp-socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/data-rate.h"

#include "storage-util.h"


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("StorageServer");

NS_OBJECT_ENSURE_REGISTERED (StorageServer);

TypeId
StorageServer::GetTypeId (void)
{
	// Note addGroup later
  static TypeId tid = TypeId ("ns3::StorageServer")
					.SetParent<Application> ()
					.AddConstructor<StorageServer> ()
					.AddAttribute ("Local",
								   "The Address on which to Bind the socket.",
								   AddressValue (),
								   MakeAddressAccessor (&StorageServer::m_local),
								   MakeAddressChecker ())
					.AddAttribute ("Protocol",
								   "The type id of the protocol to use for the socket.",
								   TypeIdValue (UdpSocketFactory::GetTypeId ()),
								   MakeTypeIdAccessor (&StorageServer::m_tid),
								   MakeTypeIdChecker ())
					.AddTraceSource ("Rx",
									 "A packet has been received",
									 MakeTraceSourceAccessor (&StorageServer::m_rxTrace),
									 "ns3::Packet::PacketAddressTracedCallback")
  ;
  return tid;
}

StorageServer::StorageServer()
{
	NS_LOG_FUNCTION (this);
	m_socket = 0;
}

StorageServer::~StorageServer()
{
	NS_LOG_FUNCTION (this);
	m_socket = 0;
}

void
StorageServer::SetLocal(const Address& address)
{
	NS_LOG_FUNCTION(this);
	m_local = address;
}

void
StorageServer::SetProtocol(const std::string protocol)
{
	NS_LOG_FUNCTION(this);
	m_tid = TypeId::LookupByName(protocol);
}

Ptr<Socket>
StorageServer::GetListeningSocket (void) const
{
  NS_LOG_FUNCTION (this);
  return m_socket;
}

std::list<Ptr<Socket> >
StorageServer::GetAcceptedSockets (void) const
{
  NS_LOG_FUNCTION (this);
  return m_socketList;
}

void
StorageServer::SetMtu(const uint16_t mtu)
{
	NS_LOG_FUNCTION(this);
	m_mtu = mtu;
}
uint16_t
StorageServer::GetMtu()
{
	NS_LOG_FUNCTION(this);
	return m_mtu;
}

void
StorageServer::DoDispose()
{
	NS_LOG_FUNCTION (this);
	m_socket = 0;
	m_socketList.clear ();
	Application::DoDispose ();
}

void
StorageServer::StartApplication (void)
{
  NS_LOG_FUNCTION (this);
  // Create the socket if not already
//  	  m_tid = TypeId::LookupByName ("ns3::TcpSocketFactory");
//  	  NS_LOG_INFO("local: " << m_local);
	if (!m_socket)
	{
		m_socket = Socket::CreateSocket(GetNode(), m_tid);
		m_socket->Bind(m_local);
		m_socket->Listen();
//		m_socket->ShutdownSend();
		if (addressUtils::IsMulticast(m_local))
		{
			Ptr<UdpSocket> udpSocket = DynamicCast<UdpSocket>(m_socket);
			if (udpSocket)
			{
				// equivalent to setsockopt (MCAST_JOIN_GROUP)
				udpSocket->MulticastJoinGroup(0, m_local);
			} else {
				NS_FATAL_ERROR("Error: joining multicast on a non-UDP socket");
			}
		}
	}

	m_socket->SetRecvCallback(MakeCallback(&StorageServer::HandleRead, this));
	m_socket->SetAcceptCallback(
			MakeNullCallback<bool, Ptr<Socket>, const Address &>(),
			MakeCallback(&StorageServer::HandleAccept, this));
	m_socket->SetCloseCallbacks(
			MakeCallback(&StorageServer::HandlePeerClose, this),
			MakeCallback(&StorageServer::HandlePeerError, this));
}

void
StorageServer::StopApplication ()
{
  NS_LOG_FUNCTION (this);
	while (!m_socketList.empty()) //these are accepted sockets, close them
	{
		Ptr<Socket> acceptedSocket = m_socketList.front();
		m_socketList.pop_front();
		acceptedSocket->Close();
	}
	if (m_socket) {
		m_socket->Close();
		m_socket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket> >());
	}
}

void
StorageServer::HandleRead (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  Ptr<Packet> packet;
    Address from;
	while ((packet = socket->RecvFrom(from)))
	{
		if (packet->GetSize() == 0)
		{ //EOF
			break;
		}

		if (InetSocketAddress::IsMatchingType(from))
		{
			NS_LOG_INFO("At time " << Simulator::Now ().GetSeconds ()
						<< "s storage server received " << packet->GetSize ()
						<< " bytes from " << InetSocketAddress::ConvertFrom(from).GetIpv4 ()
						<< " port " << InetSocketAddress::ConvertFrom (from).GetPort () );

		} else if (Inet6SocketAddress::IsMatchingType(from)) {
			NS_LOG_INFO("At time " << Simulator::Now ().GetSeconds ()
						<< "s storage server received " << packet->GetSize ()
						<< " bytes from " << Inet6SocketAddress::ConvertFrom(from).GetIpv6 ()
						<< " port " << Inet6SocketAddress::ConvertFrom (from).GetPort () );

		}
		m_rxTrace(packet, from);

		std::ostringstream *os = new std::ostringstream();
		packet->CopyData(os, packet->GetSize());
		std::string s = os->str();
		 //first splitting the string in half to access rate and required data
		NS_LOG_INFO("String received: " << s);
		std::string::size_type n = s.find_first_of("|");
		std::string firstPart = s.substr(0, n);
		std::string secondPart = s.substr(n, std::string::npos);
//		socket->Connect(from);
		ScheduleSend(socket, Storage(ExtractData(firstPart)), DataRate(ExtractData(secondPart)));
	}


}

void
StorageServer::HandlePeerClose (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

}

void
StorageServer::HandlePeerError (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
}


void
StorageServer::HandleAccept (Ptr<Socket> s, const Address& from)
{
  NS_LOG_FUNCTION (this << s << from);
  s->SetRecvCallback (MakeCallback (&StorageServer::HandleRead, this));
  m_socketList.push_back (s);
}

uint64_t
StorageServer::ExtractData(std::string s)
{
	std::string::size_type n = s.find_first_of("0123456789.");
	std::istringstream iss;
	iss.str(s.substr(n, std::string::npos));
	NS_LOG_INFO("Extracted Value: " << s.substr(n, std::string::npos));
	double r;
	iss >> r;
	return (uint64_t)r;
}

void
StorageServer::ScheduleSend(Ptr<Socket> socket, const Storage& reqData, const DataRate& rate)
{
	NS_LOG_FUNCTION(this);
	if(reqData.GetStorage() > 0)
	{
		double delay = (m_mtu * 8)/static_cast<double>(rate.GetBitRate ());
		Simulator::Schedule(Seconds(delay), &StorageServer::Send, this, socket, reqData, rate);
	}
}

void
StorageServer::Send(Ptr<Socket> socket, Storage& reqData, const DataRate& rate)
{
	NS_LOG_FUNCTION(this);
//	NS_LOG_INFO("Client: " << socket->)
	if(reqData.GetStorage() > 0)
	{
		Ptr<Packet> p;
		if(reqData.GetStorage() > m_mtu)
		{
			p = Create<Packet>(m_mtu);
			reqData = reqData - Storage(m_mtu);
		}
		else
		{
			p = Create<Packet>(reqData.GetStorage());
			reqData = Storage(0);
		}
		socket->Send(p);
	}

	if(reqData.GetStorage() > 0)
	{
		ScheduleSend(socket, reqData, rate);
	}
	else
	{
		m_socketList.remove(socket);
		socket->Close();
	}
}

} /* namespace ns3 */
