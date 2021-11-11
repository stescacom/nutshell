/*
 * consumer-producer-vm.cc
 *
 *  Created on: Aug 19, 2015
 *      Author: ubaid
 */

#include "consumer-producer-vm.h"

#include "ns3/log.h"
#include "ns3/address.h"
#include "ns3/address-utils.h"
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
#include "ns3/udp-socket-factory.h"
#include "ns3/string.h"
#include "ns3/pointer.h"
#include "ns3/boolean.h"
#include "ns3/udp-socket.h"
#include "ns3/socket-factory.h"
#include "ns3/enum.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("ConsumerProducerVm");

NS_OBJECT_ENSURE_REGISTERED(ConsumerProducerVm);


TypeId
ConsumerProducerVm::GetTypeId()
{
	static TypeId tid = ns3::TypeId("ns3::ConsumerProducerVm")
						.SetParent<NetworkVm>()
						.AddConstructor<ConsumerProducerVm>()
						.AddAttribute("ConsumerType", "Type of consumer",
										EnumValue(ConsumerProducerVm::CONSUMER_CLIENT),
										MakeEnumAccessor(&ConsumerProducerVm::m_consumerType),
										MakeEnumChecker(ConsumerProducerVm::CONSUMER_CLIENT, "ConsumerClient",
														ConsumerProducerVm::CONSUMER_CONSUMER, "ConsumerConsumer"))
						.AddAttribute("ProducerForRemoteAddress",
										"The address of remote VM host, for which this VM is a producer",
										AddressValue(),
										MakeAddressAccessor(&ConsumerProducerVm::m_producerForRemoteAddress),
										MakeAddressChecker())
						.AddAttribute("ProducerForRemotePort",
										"The port of VM to which this VM is a producer",
										UintegerValue(0),
										MakeUintegerAccessor(&ConsumerProducerVm::m_producerForRemotePort),
										MakeUintegerChecker<uint16_t>())
						.AddTraceSource ("Rx",
										 "A packet has been received",
										 MakeTraceSourceAccessor (&ConsumerProducerVm::m_rxTrace))
						;
	return tid;
}

ConsumerProducerVm::ConsumerProducerVm()
{
	NetworkVm::SetUseLocalDataProcessing(true);
	m_producing = false;
	m_requesting = false;
	m_done = false;
	SetSentBytes(Storage("0B"));
}

ConsumerProducerVm::~ConsumerProducerVm()
{
}

void
ConsumerProducerVm::SetConsumerType(ConsumerType_e type)
{
	NS_LOG_FUNCTION(this);
	m_consumerType = type;
}

void
ConsumerProducerVm::SetProducerRemoteDest(const Ipv4Address& address, const uint16_t port)
{
	m_producerForRemoteAddress = Address(address);
	m_producerForRemotePort = port;
}

void
ConsumerProducerVm::DoDispose()
{
	NS_LOG_FUNCTION(this);
	NetworkVm::DoDispose();
}

void
ConsumerProducerVm::ScheduleStop(double delay)
{
	NS_LOG_FUNCTION(this);
	Simulator::Schedule(Seconds(delay), &ConsumerProducerVm::StopApplication, this);
}

void
ConsumerProducerVm::ScheduleTransmit(double delay)
{
	NS_LOG_FUNCTION(this);
	Simulator::Schedule(Seconds(delay), &ConsumerProducerVm::BeginTransmission, this);
}
void
ConsumerProducerVm::BeginTransmission()
{
	NS_LOG_FUNCTION(this);
	NS_LOG_INFO("ConsumerProducer VM starting its data transmission\n "
				<< " at : " << Simulator::Now().GetSeconds() << "s \nTransmission Rate: "
				<< this->GetVmTransmissionRate());
	NetworkVm::BeginTransmission();
}

void
ConsumerProducerVm::HandleRead(Ptr<Socket> socket)
{
	NS_LOG_FUNCTION(this);
	NS_LOG_INFO("Consumer Handle Read");
	Ptr<Packet> packet;
	Address from;
	if(!m_producing)
	{
		while( (packet = socket->RecvFrom(from)) )
		{
			if(m_receivedBytes < GetDataSize())
			{
				m_receivedBytes = m_receivedBytes + Storage(packet->GetSize());
				if(m_receivedBytes == GetDataSize())
				{
					ExecuteOperation();
				}
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		while ((packet = socket->RecvFrom(from)))
		{
			if (packet->GetSize() == 0)
			{ //EOF
				break;
			}

			if (InetSocketAddress::IsMatchingType(from))
			{
				NS_LOG_INFO("At time " << Simulator::Now ().GetSeconds ()
							<< "s ConsumerProducerVm received " << packet->GetSize ()
							<< " bytes from " << InetSocketAddress::ConvertFrom(from).GetIpv4 ()
							<< " port " << InetSocketAddress::ConvertFrom (from).GetPort () );

			} else if (Inet6SocketAddress::IsMatchingType(from)) {
				NS_LOG_INFO("At time " << Simulator::Now ().GetSeconds ()
							<< "s ConsumerProducerVm received " << packet->GetSize ()
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


}

void
ConsumerProducerVm::CreateTransmittingSocket()
{
	NetworkVm::CreateTransmittingSocket();
	if(!m_producing)
	{
		if(m_consumerType == ConsumerProducerVm::CONSUMER_CLIENT)
		{
			m_socket->SetRecvCallback(MakeCallback(&ConsumerProducerVm::HandleRead, this));
			m_socket->SetCloseCallbacks(
							MakeCallback(&ConsumerProducerVm::HandleServerClose, this),
							MakeCallback(&ConsumerProducerVm::HandleServerCloseError, this));
		}
		else if(m_consumerType == ConsumerProducerVm::CONSUMER_CONSUMER &&
				(m_requesting || m_done))
		{
			m_socket->SetRecvCallback(MakeCallback(&ConsumerProducerVm::HandleRead, this));
			m_socket->SetCloseCallbacks(
						MakeCallback(&ConsumerProducerVm::HandleProducerClose, this),
						MakeCallback(&ConsumerProducerVm::HandleProducerCloseError, this));

		}
	}
}

void
ConsumerProducerVm::CreateListeningSocket()
{
	NetworkVm::CreateListeningSocket();
	m_listeningSocket->SetRecvCallback(MakeCallback(&ConsumerProducerVm::HandleRead, this));
	m_listeningSocket->SetAcceptCallback(
			MakeNullCallback<bool, Ptr<Socket>, const Address &>(),
			MakeCallback(&ConsumerProducerVm::HandleAccept, this));
	m_listeningSocket->SetCloseCallbacks(
			MakeCallback(&ConsumerProducerVm::HandlePeerClose, this),
			MakeCallback(&ConsumerProducerVm::HandlePeerError, this));
}

void
ConsumerProducerVm::CreateSocket()
{
	NS_LOG_FUNCTION(this);
	if(m_consumerType == ConsumerProducerVm::CONSUMER_CLIENT)
	{
		CreateTransmittingSocket();
	}
	CreateListeningSocket();

}

void
ConsumerProducerVm::CloseSocket()
{
	NS_LOG_FUNCTION(this);
	CloseTransmittingSocket();
	CloseListeningSocket();

}

void
ConsumerProducerVm::StartApplication()
{
	NS_LOG_FUNCTION(this);

	if(ReserveResources() && GetCompNode()->ReserveNicRate(GetVmTransmissionRate()))
	{
		CreateSocket();
		if(m_consumerType == ConsumerProducerVm::CONSUMER_CLIENT)
		{
			SetSocketRecvCallBack(MakeCallback(&ConsumerProducerVm::HandleRead, this));
			std::string fill;
			PrepareRequestString(fill, GetDataSize().GetStorage());
			Ptr<Packet> p = CreatePacket(fill);
			Send(p);
		}
		else if(m_consumerType == ConsumerProducerVm::CONSUMER_CONSUMER)
		{
			NS_LOG_INFO(this << "VM is waiting for producer to send its data");
		}
		else
		{
			NS_FATAL_ERROR(this << "Cannot resolve consumer type");
		}
	}
	else
	{
		StopApplication();
	}


}

void
ConsumerProducerVm::StopApplication()
{
	NS_LOG_FUNCTION(this);
	if(m_reservedResources)
	{
		ReleaseResources();
		CloseSocket();
		NS_LOG_INFO("ConsumerProducer VM stops at: " << Simulator::Now().GetSeconds());

	}

}

void
ConsumerProducerVm::PrepareRequestString(std::string& fill, uint64_t dataSize)
{
//	std::string fill =  "Required Data: " << this->GetDataSize().GetStorage() << " Rate: " << this->GetVmTransmissionRate().GetBitRate();
	fill = "RD: ";
	std::ostringstream o;
	o << dataSize;
	fill += o.str();
	if(m_consumerType == ConsumerProducerVm::CONSUMER_CLIENT)
	{
		o.str("");
		fill += "| R: ";
		o << this->GetVmTransmissionRate().GetBitRate();
		fill += o.str();
	}
}

void
ConsumerProducerVm::ExecuteOperation()
{
	NS_LOG_FUNCTION(this);
	Storage rd = this->GetDataSize();
	NS_LOG_INFO("RD: " << rd);
	double dataFetch = CalculateDataFetchTime();
	double executionTime = this->CalculateProcessingTime();
	/*
	 * Close socket, reset socket to 0 so that another socket
	 * can be created, setting m_producing to true will prevent
	 * the socket closing when application stops, to be treated as
	 * consumers.
	 * The producer part is to begin transmission to another consumer that
	 * is tied up to this VM. So we have two separate variables for the destination
	 * host address and port. We re-assign them to parent class's address and port variables
	 * then again connect the socket then begin the transmission
	 *
	 */
	CloseTransmittingSocket();

	m_producing = true;
	m_remoteAddress = m_producerForRemoteAddress;
	m_peerPort = m_producerForRemotePort;

	CreateTransmittingSocket();
	ScheduleTransmit((dataFetch+executionTime));
}

void
ConsumerProducerVm::HandleServerClose(Ptr<Socket> socket)
{
	NS_LOG_FUNCTION(this);
//	NS_LOG_INFO("Server has closed its socket" << Simulator::Now().GetSeconds());

	if(m_consumerType == ConsumerProducerVm::CONSUMER_CLIENT)
	{
		if(m_receivedBytes < GetDataSize())
		{
			CloseTransmittingSocket();
			CreateTransmittingSocket();
			Storage remaining = GetDataSize() - m_receivedBytes;
			NS_LOG_INFO("Server closed Remaining: " << remaining);
			std::string fill;
			PrepareRequestString(fill, remaining.GetStorage());
			Ptr<Packet> p = CreatePacket(fill);
			Send(p);
		}
	}
}

void
ConsumerProducerVm::HandleServerCloseError(Ptr<Socket> socket)
{
	NS_LOG_FUNCTION(this);
	if(m_done)
	{
		if(socket)
		{
			socket->Close();
		}
		return;
	}
	if(m_consumerType == ConsumerProducerVm::CONSUMER_CONSUMER)
	{
		if(m_receivedBytes < GetDataSize())
		{
			m_requesting = true;
			if(m_socket)
			{
				CloseTransmittingSocket();
			}
			CreateTransmittingSocket();

			Storage remaining = GetDataSize() - m_receivedBytes;
			NS_LOG_INFO("Producer close Remaining: " << remaining);
			std::string fill;
			PrepareRequestString(fill, remaining.GetStorage());
			Ptr<Packet> p = CreatePacket(fill);
			Send(p);
		}
		else
		{
			m_done = true;
			CloseTransmittingSocket();
			CreateTransmittingSocket();
			std::string fill = "Done";
			Ptr<Packet> p = CreatePacket(fill);
			Send(p);
//			CloseTransmittingSocket();
		}
	}
}


void
ConsumerProducerVm::HandleProducerClose(Ptr<Socket> socket)
{
	NS_LOG_FUNCTION(this);
//	NS_LOG_INFO("Server has closed its socket" << Simulator::Now().GetSeconds());
	if(m_done)
	{
		if(socket)
		{
			socket->Close();
		}
		return;
	}
	if(m_consumerType == ConsumerProducerVm::CONSUMER_CONSUMER)
	{
		if(m_receivedBytes < GetDataSize())
		{
			m_requesting = true;
			if(m_socket)
			{
				CloseTransmittingSocket();
			}
			CreateTransmittingSocket();

			Storage remaining = GetDataSize() - m_receivedBytes;
			NS_LOG_INFO("Producer close Remaining: " << remaining);
			std::string fill;
			PrepareRequestString(fill, remaining.GetStorage());
			Ptr<Packet> p = CreatePacket(fill);
			Send(p);
		}
		else
		{
			m_done = true;
			CloseTransmittingSocket();
			CreateTransmittingSocket();
			std::string fill = "Done";
			Ptr<Packet> p = CreatePacket(fill);
			Send(p);
//			CloseTransmittingSocket();
		}
	}
}

void
ConsumerProducerVm::HandleProducerCloseError(Ptr<Socket> socket)
{
	NS_LOG_FUNCTION(this);

}


/*
 * ------------------------------------Sink area--------------------------
 */


void
ConsumerProducerVm::HandlePeerClose (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  if(!m_producing)
  {
	  if(m_consumerType == ConsumerProducerVm::CONSUMER_CONSUMER)
	  	{
	  		if(m_receivedBytes < GetDataSize())
	  		{
	  			m_requesting = true;
	  			if(m_socket)
	  			{
	  				CloseTransmittingSocket();
	  			}
	  			CreateTransmittingSocket();
	  			Storage remaining = GetDataSize() - m_receivedBytes;
	  			NS_LOG_INFO("Peer close Remaining: " << remaining);
	  			std::string fill;
	  			PrepareRequestString(fill, remaining.GetStorage());
	  			Ptr<Packet> p = CreatePacket(fill);
	  			Send(p);
	  		}
	  		else
	  		{
	  			m_done = true;
	  			CloseTransmittingSocket();
	  			CreateTransmittingSocket();
	  			std::string fill = "Done";
	  			Ptr<Packet> p = CreatePacket(fill);
	  			Send(p);
	  		}
	  	}
  }

}

void
ConsumerProducerVm::HandlePeerError (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
}


void
ConsumerProducerVm::HandleAccept (Ptr<Socket> s, const Address& from)
{
  NS_LOG_FUNCTION (this << s << from);
  NS_LOG_INFO("Incomming connection at consumer producer vm");
  s->SetRecvCallback (MakeCallback (&ConsumerProducerVm::HandleRead, this));
  m_socketList.push_back (s);
}

} /* namespace ns3 */
