/*
 * network-vm.cc
 *
 *  Created on: Aug 19, 2015
 *      Author: ubaid
 */

#include "network-vm.h"

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
#include "ns3/string.h"
#include "ns3/pointer.h"
#include "ns3/boolean.h"
#include "ns3/double.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("NetworkVm");

NS_OBJECT_ENSURE_REGISTERED(NetworkVm);

TypeId
NetworkVm::GetTypeId()
{
	static TypeId tid = TypeId("ns3::NetworkVm")
						.SetParent<VirtualMachine>()
						.AddConstructor<NetworkVm>()
						.AddAttribute ("RemoteAddress",
									   "The destination Address of the outbound packets",
									   AddressValue (),
									   MakeAddressAccessor (&NetworkVm::m_remoteAddress),
									   MakeAddressChecker ())
						.AddAttribute ("RemotePort",
									   "The destination port of the outbound packets",
									   UintegerValue (0),
									   MakeUintegerAccessor (&NetworkVm::m_peerPort),
									   MakeUintegerChecker<uint16_t> ())
					   .AddAttribute ("ListeningAddress",
									   "The Address of node for inbound packets",
									   AddressValue (),
									   MakeAddressAccessor (&NetworkVm::m_listeningAddress),
									   MakeAddressChecker ())
						.AddAttribute ("ListeningPort",
									   "The port of node for inbound packets",
									   UintegerValue (0),
									   MakeUintegerAccessor (&NetworkVm::m_listeningPort),
									   MakeUintegerChecker<uint16_t> ())
					   .AddAttribute("DataSize", "Data size for transmission or requirement",
							   	   	   StorageValue(Storage("0")),
									   MakeStorageAccessor(&NetworkVm::m_dataSize),
									   MakeStorageChecker())
					   .AddAttribute("VmTransmissionRate",
							   	   	   "The Transmission rate of Virtual Machine",
									   DataRateValue(DataRate("100Kbps")),
									   MakeDataRateAccessor(&NetworkVm::m_vmTransmissionRate),
									   MakeDataRateChecker())
					   .AddAttribute("SocketType", "The TypeId of socket to use.",
							   	   	   TypeIdValue(TypeId::LookupByName("ns3::TcpSocketFactory")),
									   MakeTypeIdAccessor(&NetworkVm::m_tid),
									   MakeTypeIdChecker())
					   .AddAttribute("Mtu", "The size of maximum transmission unit",
							   	   	   UintegerValue(1500),
									   MakeUintegerAccessor(&NetworkVm::m_mtu),
									   MakeUintegerChecker<uint32_t>())
					   .AddAttribute("UseLocalDataProcessing", "Whether the VM uses local data fetching or not",
							   	   	   BooleanValue(false),
									   MakeBooleanAccessor(&NetworkVm::m_useLocalDataProcessing),
									   MakeBooleanChecker())
					   .AddAttribute("HddRwRate",
										"Rate at which data is read/written from/to secondary storage",
										StorageValue(Storage("300MB")),
										MakeStorageAccessor(&NetworkVm::m_hddRwRate),
										MakeStorageChecker())
						.AddAttribute("MemRwRate",
										"Rate at which data is read/written from/to primary storage",
										StorageValue(Storage("9600MB")),
										MakeStorageAccessor(&NetworkVm::m_memRwRate),
										MakeStorageChecker())
						.AddAttribute("NumOfAccesses",
										"Number of access to RAM by processor for data",
										UintegerValue(100),
										MakeUintegerAccessor(&NetworkVm::m_numOfAcc),
										MakeUintegerChecker<uint32_t>(1))
						.AddAttribute("RamPercentageForData",
										"Percentage of RAM available for Data storage that was read from secondary storage",
										DoubleValue(80.0),
										MakeDoubleAccessor(&NetworkVm::m_ramPDF),
										MakeDoubleChecker<double>(10.0))
						.AddAttribute("MemoryAccessTime",
										"Access time of primary storage to access the data required",
										TimeValue(NanoSeconds(100.0)),
										MakeTimeAccessor(&NetworkVm::m_memAccTime),
										MakeTimeChecker(NanoSeconds(50.0)))
						.AddAttribute("HddAccessTime",
										"Access time of secondary storage to access the required data",
										TimeValue(MilliSeconds(12)),
										MakeTimeAccessor(&NetworkVm::m_hddAccTime),
										MakeTimeChecker())
						;
	return tid;
}

NetworkVm::NetworkVm()
	: m_dataSize("0"),
	  m_vmTransmissionRate("100Kbps")
{
	NS_LOG_FUNCTION(this);
	m_mtu = 1500;
	m_useLocalDataProcessing = false;
	m_sentBytes = Storage("0B");
}

NetworkVm::~NetworkVm()
{
	NS_LOG_FUNCTION(this);
}

void
NetworkVm::SetRemote(const Ipv4Address & address, const uint16_t port)
{
	m_remoteAddress = Address(address);
	m_peerPort = port;
}

void
NetworkVm::SetListener(const Ipv4Address& address, const uint16_t port)
{
	m_listeningAddress = Address(address);
	m_listeningPort = port;
}


Address
NetworkVm::GetRemoteAddress() const
{
	return m_remoteAddress;
}

uint16_t
NetworkVm::GetPeerPort() const
{
	return m_peerPort;
}

void
NetworkVm::SetVmTransmissionRate(const DataRate & transRate)
{
	m_vmTransmissionRate = transRate;
}

DataRate
NetworkVm::GetVmTransmissionRate() const
{
	return m_vmTransmissionRate;
}

void
NetworkVm::SetDataSize(const Storage & dataSize)
{
	m_dataSize = dataSize;
}

Storage
NetworkVm::GetDataSize() const
{
	return m_dataSize;
}

bool
NetworkVm::GetConnectionStatus() const
{
	return m_connected;
}

void
NetworkVm::SetMtu(uint32_t mtu)
{
	m_mtu = mtu;
}

uint32_t
NetworkVm::GetMtu() const
{
	return m_mtu;
}

void
NetworkVm::SetSocketType(std::string t)
{
	m_tid = TypeId::LookupByName (t);;
}


void
NetworkVm::SetUseLocalDataProcessing(bool v)
{
	m_useLocalDataProcessing = v;
}

void
NetworkVm::SetHddRwRate(const Storage rate)
{
	m_hddRwRate = rate;
}
void
NetworkVm::SetMemRwRate(const Storage rate)
{
	m_memRwRate = rate;
}
void
NetworkVm::SetNumOfAccesses(const uint32_t v)
{
	m_numOfAcc = v;
}
void
NetworkVm::SetRamPDF(const double percentage)
{
	m_ramPDF = percentage;
}
void
NetworkVm::SetMemAccTime(const Time t)
{
	m_memAccTime = t;
}
void
NetworkVm::SetHddAccTime(const Time t)
{
	m_hddAccTime = t;
}

Storage
NetworkVm::GetHddRwRate() const
{
	return m_hddRwRate;
}
Storage
NetworkVm::GetMemRwRate() const
{
	return m_memRwRate;
}
uint32_t
NetworkVm::GetNumofAccesses() const
{
	return m_numOfAcc;
}
double
NetworkVm::GetRamPDF() const
{
	return m_ramPDF;
}
Time
NetworkVm::GetMemAccTime() const
{
	return m_memAccTime;
}
Time
NetworkVm::GetHddAccTime() const
{
	return m_hddAccTime;
}
bool
NetworkVm::IsUseLocalDataProcessingEnabled() const
{
	return m_useLocalDataProcessing;
}



void
NetworkVm::DoDispose()
{
	NS_LOG_FUNCTION(this);
	m_socket = 0;
	m_listeningSocket = 0;
	VirtualMachine::DoDispose();
}

/*---------------------------------------- Socket area -------------------------*/

void
NetworkVm::CreateListeningSocket()
{
	if (!m_listeningSocket)
	{
		m_listeningSocket = Socket::CreateSocket(GetNode(), m_tid);
		Address local = Address(InetSocketAddress(Ipv4Address::ConvertFrom(m_listeningAddress), m_listeningPort));
		m_listeningSocket->Bind(local);
		m_listeningSocket->Listen();
		if (addressUtils::IsMulticast(local))
		{
			Ptr<UdpSocket> udpSocket = DynamicCast<UdpSocket>(m_socket);
			if (udpSocket)
			{
				// equivalent to setsockopt (MCAST_JOIN_GROUP)
				udpSocket->MulticastJoinGroup(0, local);
			} else {
				NS_FATAL_ERROR("Error: joining multicast on a non-UDP socket");
			}
		}
	}
	/*
	 * the derived class must define its own callbacks
	 */

}

void
NetworkVm::CreateTransmittingSocket()
{
	if(!m_socket)
	{
		m_socket = Socket::CreateSocket(GetNode(), m_tid);
		if(Ipv6Address::IsMatchingType(m_remoteAddress))
		{
			m_socket->Bind6();
			m_socket->Connect(Inet6SocketAddress(Ipv6Address::ConvertFrom(m_remoteAddress), m_peerPort));
		}
		else if(Ipv4Address::IsMatchingType(m_remoteAddress))
		{
			m_socket->Bind();
			m_socket->Connect(InetSocketAddress(Ipv4Address::ConvertFrom(m_remoteAddress), m_peerPort));
		}
		m_socket->SetAllowBroadcast(true);

		m_socket->SetConnectCallback(
				MakeCallback(&NetworkVm::ConnectionSucceeded, this),
				MakeCallback(&NetworkVm::ConnectionFailed, this));

	}
}

void
NetworkVm::CreateSocket()
{
	NS_LOG_FUNCTION(this);
}

void
NetworkVm::CloseListeningSocket()
{
	while (!m_socketList.empty()) //these are accepted sockets, close them
	{
		Ptr<Socket> acceptedSocket = m_socketList.front();
		m_socketList.pop_front();
		acceptedSocket->Close();
	}
	if (m_listeningSocket)
	{
		m_listeningSocket->Close();
		m_listeningSocket->SetRecvCallback(MakeNullCallback<void, Ptr<Socket> >());
		m_listeningSocket = 0;
	}
}

void
NetworkVm::CloseTransmittingSocket()
{
	if(m_socket != 0)
	{
		m_socket->Close();
		m_socket = 0;
	}
	else
	{
		NS_LOG_WARN ("VM found null socket to close in StopApplication");
	}
}

void
NetworkVm::CloseSocket()
{
	NS_LOG_FUNCTION(this);
}

/*---------------------------------------- End of Socket area  --------------------*/


/*---------------------------------------- Handlers  --------------------*/


void
NetworkVm::RetransmitRequest(std::string s, Ptr<Socket> socket)
{

	ScheduleReTransmit(socket, Storage(ExtractData(s)));
}

/*---------------------------------------- end Handlers  --------------------*/

std::string
NetworkVm::GetPacketString(Ptr<Packet> packet)
{
	std::ostringstream *os = new std::ostringstream();
	packet->CopyData(os, (packet->GetSize() - 1));
	std::string s = os->str();
	 //first splitting the string in half to access rate and required data
	NS_LOG_INFO("String received: " << s);
	return s;
}

void
NetworkVm::SetSocketRecvCallBack(Callback<void, Ptr<Socket> > receivedData)
{
	NS_LOG_FUNCTION(this);
	m_socket->SetRecvCallback(receivedData);
}



void
NetworkVm::ConnectionSucceeded(Ptr<Socket> socket)
{
	NS_LOG_FUNCTION(this);
	m_connected = true;
}
void
NetworkVm::ConnectionFailed(Ptr<Socket> socket)
{
	NS_LOG_FUNCTION(this);
	m_connected = false;
}

Ptr<Packet>
NetworkVm::CreatePacket(std::string fill)
{
	NS_LOG_FUNCTION(this << fill);
	uint32_t dataSize = fill.size() + 1;
	uint8_t *packetData = new uint8_t[dataSize];
	memcpy(packetData, fill.c_str(), dataSize);
	Ptr<Packet> p;
	p = Create<Packet>(packetData, dataSize);
	return p;
}

Ptr<Packet>
NetworkVm::CreatePacket(Storage dataSize)
{
	Ptr<Packet> p = Create<Packet>(dataSize.GetStorage());
	return p;
}

Ptr<Packet>
NetworkVm::CreatePacket(uint32_t dataSize)
{
	Ptr<Packet> p = Create<Packet>(dataSize);
	return p;
}

void
NetworkVm::ScheduleTransmit(double delay)
{
	NS_LOG_FUNCTION(this);
	m_sendEvent = Simulator::Schedule(Seconds(delay), &NetworkVm::BeginTransmission, this);
}

void
NetworkVm::ScheduleReTransmit(Ptr<Socket> socket, Storage reqData)
{
	NS_LOG_FUNCTION(this);
	if(reqData.GetStorage() > 0)
	{
		double delay = (m_mtu * 8)/static_cast<double>(m_vmTransmissionRate.GetBitRate ());
		Simulator::Schedule(Seconds(delay), &NetworkVm::SendRt, this, socket, reqData);
	}
}

void
NetworkVm::BeginTransmission()
{
	NS_LOG_FUNCTION(this);

	if(m_connected)
	{
		if(m_sentBytes < m_dataSize)
		{

			Storage bytesLeft = m_dataSize - m_sentBytes;
			Storage cSentBytes;
			Ptr<Packet> p;

			if(bytesLeft.GetStorage() < m_mtu)
			{
				p = CreatePacket(bytesLeft);
				cSentBytes = bytesLeft;
				NS_LOG_INFO("Creating packet of " << bytesLeft);
			}
			else
			{
				p = CreatePacket(m_mtu);
				cSentBytes = Storage(m_mtu);
				NS_LOG_INFO("Creating packet of " << m_mtu);
			}
			Send(p);
			// add to m_sentBytes
			m_sentBytes = m_sentBytes + cSentBytes;

			// now to calculate time till next packet and
			// schedule it according to that time
			uint32_t bits = m_mtu * 8;
			ScheduleTransmit(bits/static_cast<double>(m_vmTransmissionRate.GetBitRate ())); // Time till next packet
		}
		else
		{
			CloseTransmittingSocket();
		}
	}
	else
	{
		NS_LOG_ERROR(this << "Cannot begin transmission, VM is not connected to destination");
	}
}
void
NetworkVm::ScheduleStop(double delay)
{
	NS_LOG_FUNCTION(this);
	Simulator::Schedule(Seconds(delay), &NetworkVm::StopApplication, this);
}
void
NetworkVm::Send(Ptr<Packet> packet)
{
	NS_LOG_FUNCTION (this);
//	m_txTrace (p);
	if(m_socket == 0)
	{
		CreateTransmittingSocket();
	}
	if(m_socket->Send (packet) != -1 )
	{
		if (Ipv4Address::IsMatchingType (m_remoteAddress))
		{
		  NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s VM sent " << packet->GetSize() << " bytes to " <<
					   Ipv4Address::ConvertFrom (m_remoteAddress) << " port " << m_peerPort);
		}
		else if (Ipv6Address::IsMatchingType (m_remoteAddress))
		{
		  NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s VM sent " << m_dataSize << " bytes to " <<
					   Ipv6Address::ConvertFrom (m_remoteAddress) << " port " << m_peerPort);
		}
	}
	else
	{
		NS_LOG_INFO("the packet was not sent " << m_socket->GetErrno());
	}


}

void
NetworkVm::SendRt(Ptr<Socket> socket, Storage reqData)
{
	NS_LOG_FUNCTION(this);
	NS_LOG_INFO("Retransmition " << reqData);
//	uint32_t newMtu = m_mtu - 100;
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
//		NS_LOG_INFO(socket->GetErrno() << " mtu : " << m_mtu << " socket error no" << Socket::ERROR_MSGSIZE);
	}

	if(reqData.GetStorage() > 0)
	{
		ScheduleReTransmit(socket, reqData);
	}
	else
	{
		m_socketList.remove(socket);
		socket->Close();
	}
}

uint64_t
NetworkVm::ExtractData(std::string s)
{
	NS_LOG_FUNCTION(this);
	std::string::size_type n = s.find_first_of("0123456789.");
	std::istringstream iss;
	iss.str(s.substr(n, std::string::npos));
	NS_LOG_INFO("Extracted Value: " << s.substr(n, std::string::npos));
	double r;
	iss >> r;
	return (uint64_t)r;
}


Ptr<Socket>
NetworkVm::GetSocket() const
{
	NS_LOG_FUNCTION(this);
	return m_socket;

}

double
NetworkVm::CalculateDataFetchTime()
{
	double totalTime = 0;

	// available RAM size for data
	uint64_t RamAvail = (GetPrimaryStorage().GetStorage() * m_ramPDF) /100 ;

	// number of access from RAM to HDD
	uint32_t numAccMem2Hdd = m_dataSize.GetStorage()/RamAvail;

	// data size per access from RAM to Processor
	uint64_t ram2ProcessorDataSize = m_dataSize.GetStorage() / m_numOfAcc;

	/*
	 * Formulas
	 * total time of Data fetch from HDD to RAM = (no of HDD access * HDD access time) + (available RAM size / HDD RW rate)
	 *
	 * total time of data fetch from RAM to Processor = (no of RAM access * RAM access time) + (RAM to Processor read data size / RAM RW rate)
	 */

	double tHdd2RAM, tRAM2Proc;

	tHdd2RAM = numAccMem2Hdd * (m_hddAccTime.GetSeconds() + (RamAvail / m_hddRwRate.GetStorage()));
	tRAM2Proc = m_numOfAcc * (m_memAccTime.GetSeconds() + (ram2ProcessorDataSize / m_memRwRate.GetStorage()));

	totalTime = tHdd2RAM + tRAM2Proc;

	return totalTime;
}

Storage
NetworkVm::GetSentBytes() const
{
	return m_sentBytes;
}
void
NetworkVm::SetSentBytes(Storage s)
{
	m_sentBytes = s;
}
void
NetworkVm::StartApplication(void)
{
	NS_LOG_FUNCTION(this);
	NS_LOG_INFO("NetworkVM: connection Status: " << m_connected);
}

void
NetworkVm::StopApplication(void)
{
	NS_LOG_FUNCTION(this);
}

} /* namespace ns3 */
