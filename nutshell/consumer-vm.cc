/*
 * consumer-vm.cc
 *
 *  Created on: Aug 19, 2015
 *      Author: ubaid
 */

#include "consumer-vm.h"

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

NS_LOG_COMPONENT_DEFINE("ConsumerVm");

NS_OBJECT_ENSURE_REGISTERED(ConsumerVm);

TypeId
ConsumerVm::GetTypeId()
{
	static TypeId tid = ns3::TypeId("ns3::ConsumerVm")
						.SetParent<NetworkVm>()
						.AddConstructor<ConsumerVm>()
						.AddAttribute("ConsumerType", "Type of consumer",
										EnumValue(ConsumerVm::CONSUMER_CLIENT),
										MakeEnumAccessor(&ConsumerVm::m_consumerType),
										MakeEnumChecker(ConsumerVm::CONSUMER_CLIENT, "ConsumerClient",
														ConsumerVm::CONSUMER_CONSUMER, "ConsumerConsumer"))
						.AddTraceSource ("Rx",
										 "A packet has been received",
										 MakeTraceSourceAccessor (&ConsumerVm::m_rxTrace),
										 "ns3::Packet::PacketAddressTracedCallback")
						;
	return tid;
}

ConsumerVm::ConsumerVm()
{
	NS_LOG_FUNCTION(this);
	NetworkVm::SetUseLocalDataProcessing(true);
	m_requesting = false;
}

ConsumerVm::~ConsumerVm()
{
	NS_LOG_FUNCTION(this);
}


void
ConsumerVm::SetConsumerType(ConsumerType_e type)
{
	NS_LOG_FUNCTION(this);
	m_consumerType = type;
}

void
ConsumerVm::DoDispose()
{
	NS_LOG_FUNCTION(this);
	NetworkVm::DoDispose();
}

void
ConsumerVm::ScheduleStop(double delay)
{
	NS_LOG_FUNCTION(this);
	Simulator::Schedule(Seconds(delay), &ConsumerVm::StopApplication, this);
}

void
ConsumerVm::HandleRead(Ptr<Socket> socket)
{
	NS_LOG_FUNCTION(this);
	Ptr<Packet> packet;
	Address from;
	while( (packet = socket->RecvFrom(from)) )
	{
//
		if(m_receivedBytes < GetDataSize())
		{

			m_receivedBytes = m_receivedBytes + Storage(packet->GetSize());
			NS_LOG_INFO("At time "<< Simulator::Now().GetSeconds() << "s Consumer VM Received : " << packet->GetSize()
							<< "Bytes, total received " << m_receivedBytes);
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

void
ConsumerVm::CreateListeningSocket()
{
	NetworkVm::CreateListeningSocket();
	m_listeningSocket->SetRecvCallback(MakeCallback(&ConsumerVm::HandleRead, this));
	m_listeningSocket->SetAcceptCallback(
			MakeNullCallback<bool, Ptr<Socket>, const Address &>(),
			MakeCallback(&ConsumerVm::HandleAccept, this));
	m_listeningSocket->SetCloseCallbacks(
			MakeCallback(&ConsumerVm::HandlePeerClose, this),
			MakeCallback(&ConsumerVm::HandlePeerError, this));
}

void
ConsumerVm::CreateTransmittingSocket()
{
	NetworkVm::CreateTransmittingSocket();
	if(m_consumerType == ConsumerVm::CONSUMER_CLIENT)
	{
		m_socket->SetRecvCallback(MakeCallback(&ConsumerVm::HandleRead, this));
		m_socket->SetCloseCallbacks(
						MakeCallback(&ConsumerVm::HandleServerClose, this),
						MakeCallback(&ConsumerVm::HandleServerCloseError, this));
	}
	else if(m_consumerType == ConsumerVm::CONSUMER_CONSUMER &&
			(m_requesting || m_done))
	{
		m_socket->SetRecvCallback(MakeCallback(&ConsumerVm::HandleRead, this));
		m_socket->SetCloseCallbacks(
					MakeCallback(&ConsumerVm::HandleProducerClose, this),
					MakeCallback(&ConsumerVm::HandleProducerCloseError, this));

	}
}

void
ConsumerVm::CreateSocket()
{
	NS_LOG_FUNCTION(this);
	if(m_consumerType == ConsumerVm::CONSUMER_CLIENT)
	{
		CreateTransmittingSocket();
	}
	CreateListeningSocket();
}

void
ConsumerVm::CloseSocket()
{
	NS_LOG_FUNCTION(this);
	if(m_socket)
	{
		CloseTransmittingSocket();
	}
	CloseListeningSocket();
}

void
ConsumerVm::StartApplication()
{
	NS_LOG_FUNCTION(this);

	if(ReserveResources() && GetCompNode()->ReserveNicRate(GetVmTransmissionRate()))
	{
		CreateSocket();
		if(m_consumerType == ConsumerVm::CONSUMER_CLIENT)
		{
			SetSocketRecvCallBack(MakeCallback(&ConsumerVm::HandleRead, this));
			std::string fill;
			PrepareRequestString(fill, GetDataSize().GetStorage());
			Ptr<Packet> p = CreatePacket(fill);
			Send(p);
		}
		else if(m_consumerType == ConsumerVm::CONSUMER_CONSUMER)
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
ConsumerVm::StopApplication()
{
	NS_LOG_FUNCTION(this);
	if(m_reservedResources)
	{
		ReleaseResources();
		NS_LOG_INFO("Consumer VM stops at: " << Simulator::Now().GetSeconds());
		CloseSocket();
	}

}

void
ConsumerVm::PrepareRequestString(std::string& fill, uint64_t dataSize)
{
//	std::string fill =  "Required Data: " << this->GetDataSize().GetStorage() << " Rate: " << this->GetVmTransmissionRate().GetBitRate();
	fill = "RD: ";
	std::ostringstream o;
	o << dataSize;
	fill += o.str();
	if(m_consumerType == ConsumerVm::CONSUMER_CLIENT)
	{
		o.str("");
		fill += "| R: ";
		o << this->GetVmTransmissionRate().GetBitRate();
		fill += o.str();
	}
}



void
ConsumerVm::ExecuteOperation()
{
	NS_LOG_FUNCTION(this);
	CloseSocket();
	double dataFetch = CalculateDataFetchTime();
	double executionTime = this->CalculateProcessingTime();
	ScheduleStop((dataFetch + executionTime));
}

void
ConsumerVm::HandleServerClose(Ptr<Socket> socket)
{
	NS_LOG_FUNCTION(this);
//	NS_LOG_INFO("Server has closed its socket" << Simulator::Now().GetSeconds());

	if(m_consumerType == ConsumerVm::CONSUMER_CLIENT)
	{
		if(m_receivedBytes < GetDataSize())
		{
			if(m_socket)
			{
				CloseTransmittingSocket();
			}
			CreateTransmittingSocket();

			Storage remaining = GetDataSize() - m_receivedBytes;
			NS_LOG_INFO("Server close Remaining: " << remaining);
			std::string fill;
			PrepareRequestString(fill, remaining.GetStorage());
			Ptr<Packet> p = CreatePacket(fill);
			Send(p);
		}
	}
}

void
ConsumerVm::HandleServerCloseError(Ptr<Socket> socket)
{
	NS_LOG_FUNCTION(this);

}


void
ConsumerVm::HandleProducerClose(Ptr<Socket> socket)
{
	NS_LOG_FUNCTION(this);
//	NS_LOG_INFO("Server has closed its socket" << Simulator::Now().GetSeconds());
	if(m_done)
	{
		return;
	}
	if(m_consumerType == ConsumerVm::CONSUMER_CONSUMER)
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
			NS_LOG_WARN("Producer close Remaining: " << remaining);
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

void
ConsumerVm::HandleProducerCloseError(Ptr<Socket> socket)
{
	NS_LOG_FUNCTION(this);

}


/*
 * ------------------------------------Sink area--------------------------
 */

void
ConsumerVm::HandlePeerClose (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
	if(m_consumerType == ConsumerVm::CONSUMER_CONSUMER)
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
			NS_LOG_INFO("ConsumerVM Peer close Remaining: " << remaining);
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

void
ConsumerVm::HandlePeerError (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  if(m_done)
  	{
  		if(socket)
  		{
  			socket->Close();
  		}
  		return;
  	}
  	if(m_consumerType == ConsumerVm::CONSUMER_CONSUMER)
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
  		}
  	}
}


void
ConsumerVm::HandleAccept (Ptr<Socket> s, const Address& from)
{
  NS_LOG_FUNCTION (this << s << from);
  s->SetRecvCallback (MakeCallback (&ConsumerVm::HandleRead, this));
  m_socketList.push_back (s);
}



} /* namespace ns3 */
