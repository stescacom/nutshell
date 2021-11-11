/*
 * producer-vm.h
 *
 *  Created on: Aug 19, 2015
 *      Author: ubaid
 */

#ifndef PRODUCER_VM
#define PRODUCER_VM

#include "network-vm.h"
#include "ns3/application.h"
#include "ns3/address.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/data-rate.h"
#include "ns3/traced-callback.h"


namespace ns3 {



class ProducerVm : public NetworkVm {
public:
	static TypeId GetTypeId(void);
	ProducerVm();
	virtual ~ProducerVm();
protected:
	virtual void DoDispose();

	virtual void CreateSocket();
	virtual void CreateListeningSocket();
	virtual void CreateTransmittingSocket();
	virtual void CloseSocket();

	virtual void ScheduleTransmit(double delay);
	virtual void BeginTransmission();
	virtual void ScheduleStop(double delay);

private:

	virtual void StartApplication (void);    // Called at time specified by Start
	virtual void StopApplication (void);     // Called at time specified by Stop

	/**
	* \brief Handle a packet received by the application
	* \param socket the receiving socket
	*/
	void HandleRead (Ptr<Socket> socket);

	/**
	* \brief Handle an incoming connection
	* \param socket the incoming connection socket
	* \param from the address the connection is from
	*/
	void HandleAccept (Ptr<Socket> socket, const Address& from);
	/**
	* \brief Handle an connection close
	* \param socket the connected socket
	*/
	void HandlePeerClose (Ptr<Socket> socket);
	/**
	* \brief Handle an connection error
	* \param socket the connected socket
	*/
	void HandlePeerError (Ptr<Socket> socket);

	/// Traced Callback: received packets, source address.
	TracedCallback<Ptr<const Packet>, const Address &> m_rxTrace;
};

} /* namespace ns3 */

#endif /* SCRATCH_C_NODE_BLOCK_PRODUCER_VM_H_ */
