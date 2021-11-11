/*
 * consumer-vm.h
 *
 *  Created on: Aug 19, 2015
 *      Author: ubaid
 */

#ifndef CONSUMER_VM
#define CONSUMER_VM

#include "network-vm.h"

#include "ns3/application.h"
#include "ns3/address.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/data-rate.h"
#include "ns3/traced-callback.h"

namespace ns3 {

class Address;
class Socket;
class Packet;
/**
 * \brief The class create a VM that consumes data.
 *
 * The VM created with this class type consumes data from another
 * source, either a Storage Server or another Producer VM. The VM waits
 * for the complete data to arrive, then continue its execution.
 *
 */
class ConsumerVm: public NetworkVm {
public:
	/**
	 * \brief The ENUM defines the VM consumer part type
	 * whether to consume from a server or another producer.
	 */
	enum ConsumerType_e
	{
		CONSUMER_CLIENT = 32,
		CONSUMER_CONSUMER = 33
	};
	/**
	* \brief Get the type ID.
	* \return the object TypeId
	*/
	static TypeId GetTypeId();
	/**
	 * \brief Class constructor
	 */
	ConsumerVm();
	virtual ~ConsumerVm();
	/**
	 * \brief Sets the consumer part type
	 * \param type The ENUM value of consumer type
	 */
	void SetConsumerType(ConsumerType_e type);


	// static members
//	static const uint16_t CONSUMER_CLIENT = 32;
//	static const uint16_t CONSUMER_CONSUMER = 33;

protected:
	virtual void DoDispose();
	/**
	 * \brief Schedules VM to stop itself.
	 * \param delay The delay amount after which the the VM shuts down.
	 */
	virtual void ScheduleStop(double delay);
	/**
	 * \brief Create the sockets required for successful data exchange
	 *
	 * The method creates two sockets at a time, one for transmission
	 * and other for listening. the listening port receives data
	 * from other VMs or Server. The transmission port sends a request
	 * for data.
	 */
	virtual void CreateSocket();
	/**
	 * \brief Creates the Listening Socket
	 */
	virtual void CreateListeningSocket();
	/**
	 * \brief Creates the Transmitting Socket
	 */
	virtual void CreateTransmittingSocket();
	/**
	 * \brief Closes both listening and transmitting sockets
	 */
	virtual void CloseSocket();

private:
	virtual void StartApplication (void);
	virtual void StopApplication (void);
	/**
	 * \brief Prepares the request string
	 *
	 * The request string is of a specific format
	 * RD: xx | R: xx
	 * * RD is Required Data
	 * * R is transmission rate at which the VM can receive data
	 *
	 */
	void PrepareRequestString(std::string& fill, uint64_t dataSize);
	/**
	 * \brief Executes the application of VM
	 */
	void ExecuteOperation();
	/**
	 * \brief Callback method that gets executed when the
	 * peer server closes its socket due to a
	 * connection interruption or when the sender is done
	 * sending data
	 * \param socket the socket that gets close
	 */
	void HandleServerClose(Ptr<Socket>);
	/**
	 * \brief Callback method that gets executed when there is an error in
	 * peer server socket close.
	 * \param socket the socket that gets close
	 */
	void HandleServerCloseError(Ptr<Socket>);
	/**
	 * \brief Callback method that gets executed when the
	 * peer producer closes its socket due to a
	 * connection interruption or when the sender is done
	 * sending data
	 * \param socket the socket that gets close
	 */
	void HandleProducerClose(Ptr<Socket>);
	/**
	 * \brief Callback method that gets executed when there is an error in
	 * peer producer socket close.
	 * \param socket the socket that gets close
	 */
	void HandleProducerCloseError(Ptr<Socket>);
	/**
	 * \brief Callback to handle packet read
	 * \param socket the socket on which packet is received
	 */
	void HandleRead(Ptr<Socket> socket);

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

	ConsumerType_e			m_consumerType; //!< Store consumer type for VM
	Storage					m_receivedBytes; //!< Keep tracks of received amount of data
	bool					m_requesting; //!< Flag to show if VM is requesting data
	bool					m_done; //!< Flag to show if the VM is done receiving

	/// Traced Callback: received packets, source address.
	TracedCallback<Ptr<const Packet>, const Address &> m_rxTrace;

};

} /* namespace ns3 */

#endif /* SCRATCH_C_NODE_BLOCK_CONSUMER_VM_H_ */
