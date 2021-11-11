/*
 * storage-server.h
 *
 *  Created on: Aug 17, 2015
 *      Author: ubaid
 */

#ifndef STORAGE_SERVER_H
#define STORAGE_SERVER_H

#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"
#include "ns3/address.h"
#include "ns3/data-rate.h"

#include "storage-util.h"

namespace ns3 {

class Address;
class Socket;
class Packet;

class StorageServer: public Application {
public:
	/**
	* \brief Get the type ID.
	* \return the object TypeId
	*/
	static TypeId GetTypeId (void);
	StorageServer();
	virtual ~StorageServer();

	/**
	* \brief Set local address
	* \param address of socket
	*/
	void SetLocal(const Address& address);

	/**
	 * \brief Set protocol to use
	 * \param protocol a string value of TypeId
	 */
	void SetProtocol(const std::string protocol);

	/**
	   * \return pointer to listening socket
	   */
	  Ptr<Socket> GetListeningSocket (void) const;

	  /**
	   * \return list of pointers to accepted sockets
	   */
	  std::list<Ptr<Socket> > GetAcceptedSockets (void) const;

	  void SetMtu(const uint16_t mtu);
	  uint16_t GetMtu();

protected:
	virtual void DoDispose (void);
private:
	// inherited from Application base class.
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

	  /**
	   * \brief Extract data from string i.e. extract Required Data and Rate of VM Request
	   * \param s a string value of specific format
	   */
	  uint64_t ExtractData(std::string s);

	  /**
	   * \brief
	   */
	  void ScheduleSend(Ptr<Socket> socket, const Storage& reqData, const DataRate& rate);

	  /**
	   * \brief
	   */
	  void Send(Ptr<Socket> socket, Storage& reqData, const DataRate& rate);

	  // In the case of TCP, each socket accept returns a new socket, so the
	  // listening socket is stored separately from the accepted sockets
	  Ptr<Socket>     m_socket;       //!< Listening socket
	  std::list<Ptr<Socket> > m_socketList; //!< the accepted sockets

	  Address         	m_local;        //!< Local address to bind to
	  TypeId          	m_tid;          //!< Protocol TypeId
	  uint16_t			m_mtu;

	  /// Traced Callback: received packets, source address.
	  TracedCallback<Ptr<const Packet>, const Address &> m_rxTrace;
};

} /* namespace ns3 */

#endif /* STORAGE_SERVER_H */
