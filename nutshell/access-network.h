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
 * access-network.h
 *
 *  Created on: Oct 21, 2015
 *      Author: ubaid
 */

#ifndef ACCESS_NETWORK
#define ACCESS_NETWORK

#include "ns3/object.h"
#include "ns3/node-container.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-interface-container.h"

#include "connection-helper.h"

namespace ns3 {
/**
 * \brief Creates a basic access network
 *
 * The class is responsible for creating a simple star topology
 * from supplied nodes and an access switch/router (also a Node pointer)
 * The class require user to provide configured Internet Stack (InternetStackHelper) and
 * point-to-point channel (PointToPointHelper), as this class will install both of them
 * while creating the topology.
 */
class AccessNetwork : public Object {
public:
    /**
    * \brief Get the type ID.
    * \return the object TypeId
    */
    static TypeId GetTypeId(void);
    /**
     * \brief Class constructor
     */
	AccessNetwork();
	/**
	 * \brief Class constructor
	 * \param nodes a node container containing node to be connected to a router/switch
	 * \param p2p a point-to-point helper, which is used as a link between the nodes
	 * \param stack the Internet stack to install on nodes
	 * \param networkAdd the network address to assign IPs from the subnet
	 * \param subnet the subnet mask of network
	 */
	AccessNetwork(NodeContainer &nodes,
					PointToPointHelper &p2p,
					InternetStackHelper &stack,
					std::string networkAdd,
					std::string subnet);
	/**
	 * \brief Begin the network creation process
	 */
	void CreateNetwork();
	/**
	 * \brief Setter for node container, containing nodes to be connected to a router/switch
	 * \param nodes node container
	 */
	void SetNodes(NodeContainer &nodes);
	/**
	 * \brief Setter for channel between nodes and router/switch
	 * \param p2p point to point helper
	 */
	void SetChannel(PointToPointHelper &p2p);
	/**
	 * \brief Setter for the Internet stack to be installed on the nodes
	 * \param stack Internet stack
	 */
	void SetIntenetStack(InternetStackHelper &stack);
	/**
	 * \brief Setter for network, which is used to assign IPs to NetDevices
	 * \param networkAdd, the subnet network address
	 * \param subnet, the network mask of subnet
	 */
	void SetNetwork(std::string networkAdd, std::string subnet);
	/**
	 * \brief Getter for the access network switch/router
	 * \return node pointer
	 */
	Ptr<Node> GetAccessSwitch() const;
	/**
	 * \brief Getter for network address
	 * \return subnet network address string value
	 */
	std::string GetNetworkAddress() const;
	/**
	 * \brief Getter for network subnet mask
	 * \return subnet mask string
	 */
	std::string GetSubnet() const;
	/**
	 * \brief Getter for the list of interfaces of end nodes except switch/router
	 * \return an interface container
	 */
	Ipv4InterfaceContainer GetTerminalInterfaces() const;
	/**
	 * \brief Getter for all interfaces of the network
	 * \return an interface container
	 */
	Ipv4InterfaceContainer GetAllInterfaces() const;
	virtual ~AccessNetwork();
private:
	/**
	 * \brief Connects nodes to switch
	 */
	void ConnectNodesToSwitch();

	Ptr<Node> m_accessSwitch; //!< access router/switch Node to which other nodes are attached to
	NodeContainer m_nodes; //!< Node list that are to be connected to access router/switch
	PointToPointHelper m_p2p; //!< Connection channel between nodes
	InternetStackHelper m_istack; //!< Internet stack for nodes
	Ipv4InterfaceContainer m_connectedDevices; //!< List of all connected devices interfaces
	Ipv4InterfaceContainer m_terminalDevices; //!< List of end node interfaces except the access router/switch
	std::string m_networkAddress; //!< Network subnet's network address
	std::string m_subnetMask; //!< Network subnet mask
};

} /* namespace ns3 */

#endif /* ACCESS_NETWORK */
