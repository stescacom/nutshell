/*
 * producer-vm.cc
 *
 *  Created on: Aug 19, 2015
 *      Author: ubaid
 */

#include "producer-vm.h"
#include "ns3/log.h"
#include "ns3/address.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/packet-socket-address.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/random-variable-stream.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/string.h"
#include "ns3/pointer.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("ProducerVm");

NS_OBJECT_ENSURE_REGISTERED(ProducerVm);

TypeId
ProducerVm::GetTypeId()
{
	static TypeId tid = ns3::TypeId("ns3::ProducerVm")
						.SetParent<NetworkVm>()
						.AddConstructor<ProducerVm>()
						;
	return tid;
}

ProducerVm::ProducerVm()
{
}

ProducerVm::~ProducerVm()
{
	NS_LOG_FUNCTION(this);
	NetworkVm::DoDispose();
}

void
ProducerVm::DoDispose()
{
	NS_LOG_FUNCTION(this);
	NetworkVm::DoDispose();
}


/*---------------------------------- socket area --------------------------------*/


void
ProducerVm::CreateSocket()
{
	CreateTransmittingSocket();
	CreateListeningSocket();
}
void
ProducerVm::CreateListeningSocket()
{
	NetworkVm::CreateListeningSocket();
	if(m_listeningSocket)
	{
		m_listeningSocket->SetRecvCallback(MakeCallback(&ProducerVm::HandleRead, this));
		m_listeningSocket->SetAcceptCallback(
								MakeNullCallback<bool, Ptr<Socket>, const Address &>(),
								MakeCallback(&ProducerVm::HandleAccept, this));
		m_listeningSocket->SetCloseCallbacks(
								MakeCallback(&ProducerVm::HandlePeerClose, this),
								MakeCallback(&ProducerVm::HandlePeerError, this));
	}
}
void
ProducerVm::CreateTransmittingSocket()
{
	NetworkVm::CreateTransmittingSocket();
}
void
ProducerVm::CloseSocket()
{
	NS_LOG_FUNCTION(this);
	CloseTransmittingSocket();
	CloseListeningSocket();
}



/*---------------------------------- end of socket area --------------------------------*/


/*------------------------------ Handle connection and packet area ---------------------*/

void
ProducerVm::HandleRead(Ptr<Socket> socket)
{
	NS_LOG_FUNCTION(this);
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
						<< "s ProducerVm received " << packet->GetSize ()
						<< " bytes from " << InetSocketAddress::ConvertFrom(from).GetIpv4 ()
						<< " port " << InetSocketAddress::ConvertFrom (from).GetPort () );

		} else if (Inet6SocketAddress::IsMatchingType(from)) {
			NS_LOG_INFO("At time " << Simulator::Now ().GetSeconds ()
						<< "s ProducerVm received " << packet->GetSize ()
						<< " bytes from " << Inet6SocketAddress::ConvertFrom(from).GetIpv6 ()
						<< " port " << Inet6SocketAddress::ConvertFrom (from).GetPort () );

		}
		m_rxTrace(packet, from);
		std::string s = GetPacketString(packet);
		if(s == "Done" || s == "done" || s == "don")
		{
			ScheduleStop(0.0);
		}
		else
		{
			RetransmitRequest(s, socket);
		}

	}
}

void
ProducerVm::HandlePeerClose (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

}

void
ProducerVm::HandlePeerError (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
}


void
ProducerVm::HandleAccept (Ptr<Socket> s, const Address& from)
{
  NS_LOG_FUNCTION (this << s << from);
  s->SetRecvCallback (MakeCallback (&ProducerVm::HandleRead, this));
  m_socketList.push_back (s);
}



void
ProducerVm::ScheduleTransmit(double delay)
{
	NS_LOG_FUNCTION(this);
	Simulator::Schedule(Seconds(delay), &ProducerVm::BeginTransmission, this);
}
void
ProducerVm::BeginTransmission()
{
	NS_LOG_FUNCTION(this);
	NS_LOG_INFO("Producer VM transmitting: "
				<< " \nat : " << Simulator::Now().GetSeconds() << "s \nTransmission Rate: "
				<< this->GetVmTransmissionRate());
	NetworkVm::BeginTransmission();
}
void
ProducerVm::ScheduleStop(double delay)
{
	NS_LOG_FUNCTION(this);
	Simulator::Schedule(Seconds(delay), &ProducerVm::StopApplication, this);
}

void
ProducerVm::StartApplication(void)
{
	NS_LOG_FUNCTION(this);
	if(ReserveResources() && GetCompNode()->ReserveNicRate(GetVmTransmissionRate()))
	{
		double delay = this->CalculateProcessingTime();
		NS_LOG_INFO("Producer VM "<< this << " Started its application execution");
		CreateSocket();
		ScheduleTransmit(delay);
	}
	else
	{
		StopApplication();
	}

}

void
ProducerVm::StopApplication(void)
{
	NS_LOG_FUNCTION(this);
	if(m_reservedResources)
	{
		ReleaseResources();
		CloseSocket();
		NS_LOG_INFO("VM stops at" << Simulator::Now());
	}

}

} /* namespace ns3 */
