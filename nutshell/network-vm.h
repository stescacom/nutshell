/*
 * network-vm.h
 *
 *  Created on: Aug 19, 2015
 *      Author: ubaid
 */

#ifndef NETWORK_VM_H
#define NETWORK_VM_H

#include "storage-util.h"
#include "virtual-machine.h"
#include "ns3/application.h"
#include "ns3/address.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/data-rate.h"
#include "ns3/traced-callback.h"



namespace ns3 {

class Address;
class RandomVariableStream;
class Socket;

class NetworkVm: public VirtualMachine {
public:
	static TypeId GetTypeId (void);
	NetworkVm();
	virtual ~NetworkVm();

	void SetRemote(const Ipv4Address& address, const uint16_t port);
	void SetListener(const Ipv4Address& address, const uint16_t port);
	void SetVmTransmissionRate(const DataRate& transRate);
	void SetDataSize(const Storage & dataSize);
	void SetMtu(uint32_t mtu);
	void SetSocketType(std::string t);

	void SetUseLocalDataProcessing(bool v);

	void SetHddRwRate(const Storage rate);
	void SetMemRwRate(const Storage rate);
	void SetNumOfAccesses(const uint32_t v);
	void SetRamPDF(const double percentage);
	void SetMemAccTime(const Time t);
	void SetHddAccTime(const Time t);

	Address GetRemoteAddress() const;
	uint16_t GetPeerPort() const;
	DataRate GetVmTransmissionRate() const;
	Storage GetDataSize() const;
	bool GetConnectionStatus() const;
	uint32_t GetMtu() const;

	bool IsUseLocalDataProcessingEnabled() const;
	Storage GetHddRwRate() const;
	Storage GetMemRwRate() const;
	uint32_t GetNumofAccesses() const;
	double GetRamPDF() const;
	Time GetMemAccTime() const;
	Time GetHddAccTime() const;



protected:
	virtual void DoDispose();

	virtual void CreateSocket();
	virtual void CreateListeningSocket();
	virtual void CreateTransmittingSocket();
	virtual void CloseSocket();
	virtual void CloseListeningSocket();
	virtual void CloseTransmittingSocket();

	virtual void ConnectionSucceeded(Ptr<Socket> socket);
	virtual void ConnectionFailed(Ptr<Socket> socket);

	virtual Ptr<Packet> CreatePacket(std::string fill);
	virtual Ptr<Packet> CreatePacket(Storage dataSize);
	virtual Ptr<Packet> CreatePacket(uint32_t dataSize);

	virtual void ScheduleTransmit(double delay);
	virtual void ScheduleReTransmit(Ptr<Socket> socket, Storage reqData);

	virtual void BeginTransmission();
	virtual void ScheduleStop(double delay);

	virtual void Send(Ptr<Packet> packet);
	virtual void SendRt(Ptr<Socket> packet, Storage reqData);

	virtual void SetSocketRecvCallBack(Callback<void, Ptr<Socket> > receivedData);

	Ptr<Socket> GetSocket() const;

	double CalculateDataFetchTime();

	Storage GetSentBytes() const;
	void SetSentBytes(Storage s);

	void RetransmitRequest(std::string s, Ptr<Socket> socket);

	std::string GetPacketString(Ptr<Packet> packet);

	/**
	* \brief Extract data from string i.e. extract Required Data and Rate of VM Request
	* \param s a string value of specific format
	*/
	uint64_t ExtractData(std::string s);

	TypeId				m_tid;
	Ptr<Socket>			m_socket;
	Ptr<Socket>			m_listeningSocket;

	/*
	 * the remote address and peer port is for
	 * either requesting data from server
	 * or a VM's producer
	 */

	Address 			m_remoteAddress;
	uint16_t 			m_peerPort; //!< Remote peer port

	/*
	 * the listening address and port are for creating
	 * a listening socket. The socket will listen to a request
	 * from consumer
	 */

	Address				m_listeningAddress;
	uint16_t			m_listeningPort;



	std::list<Ptr<Socket> > m_socketList; //!< the accepted sockets

private:

	Storage				m_dataSize;
	Storage				m_sentBytes;


	DataRate			m_vmTransmissionRate;
	EventId				m_requestEvent;
	EventId				m_sendEvent;
	bool 				m_connected;
	uint32_t			m_mtu;

	bool				m_useLocalDataProcessing;
	Storage				m_hddRwRate;
	Storage				m_memRwRate;

	uint32_t 			m_numOfAcc;	// processor request for data
	double				m_ramPDF;	// ram percentage available for data fetching

	Time				m_memAccTime;
	Time				m_hddAccTime;


	// inherited from Application base class.
	virtual void StartApplication (void);    // Called at time specified by Start
	virtual void StopApplication (void);     // Called at time specified by Stop

};

} /* namespace ns3 */

#endif /* NETWORK_VM_H */
