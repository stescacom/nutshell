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
 * connection-helper.h
 *
 *  Created on: Oct 6, 2015
 *      Author: ubaid
 */

#ifndef CONNECTION_HELPER_H
#define CONNECTION_HELPER_H

#include <vector>

#include "ns3/node.h"
#include "ns3/pointer.h"
#include "ns3/node-container.h"
#include "ns3/csma-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/bridge-helper.h"

namespace ns3 {
/**
 * \brief The helper creates a connection between
 * switch/router and nodes. The class also assign address
 * to each node.
 */
class ConnectionHelper {
public:
	/**
	 * \brief Connects devices with layer 2 channel
	 * \param l2switch The switch to which the nodes will connect
	 * \param connectingDevices The nodes to be connected to the switch
	 * \param csma The configured CSMA link
	 * \param networkAddress Network Subnet network address
	 * \param subnet Netowrk Subnet's subnet mask.
	 */
	void ConnectDevicesLayer2(Ptr<Node>& l2switch,
										NodeContainer& connectingDevices,
										CsmaHelper& csma,
										std::string networkAddress, std::string subnet);
	/**
	 * \brief Connects devices with layer 2 channel and also send out the node interface container
	 * \param l2switch The switch to which the nodes will connect
	 * \param connectingDevices The nodes to be connected to the switch
	 * \param csma The configured CSMA link
	 * \param terminalInterfaces The Node's IP interface container
	 * \param networkAddress Network Subnet network address
	 * \param subnet Netowrk Subnet's subnet mask.
	 */
	void ConnectDevicesLayer2(Ptr<Node>& l2switch,
										NodeContainer& connectingDevices,
										CsmaHelper& csma,
										Ipv4InterfaceContainer& terminalInterfaces,
										std::string networkAddress, std::string subnet);
	/**
	 * \brief Connects devices with layer 2 channel and also send out the node interface container, the bridge created
	 * NetDevices on the bridge, and terminal NetDevices as well.
	 * \param l2switch The switch to which the nodes will connect
	 * \param connectingDevices The nodes to be connected to the switch
	 * \param csma The configured CSMA link
	 * \param bridge The bridge to be used
	 * \param bridgeDevices The bridge NetDevice container
	 * \param terminalDevices The Nodes NetDevice container
	 * \param terminalInterfaces The Node's IP interface container
	 * \param networkAddress Network Subnet network address
	 * \param subnet Netowrk Subnet's subnet mask.
	 */
	void ConnectDevicesLayer2(Ptr<Node>& l2switch,
										NodeContainer& connectingDevices,
										CsmaHelper& csma,
										BridgeHelper& bridge,
										NetDeviceContainer& bridgeDevices,
										NetDeviceContainer& terminalDevices,
										Ipv4InterfaceContainer& terminalInterfaces,
										std::string networkAddress, std::string subnet);

//	void ConnectDevicesLayer3(Ptr<Node>& l3device,
//								NodeContainer& connectingDevices,
//								PointToPointHelper& p2p,
//								std::vector< std::vector< std::string > >& networkAddr);
//
//	void ConnectDevicesLayer3(Ptr<Node>& l3device,
//								NodeContainer& connectingDevices,
//								PointToPointHelper& p2p,
//								Ipv4InterfaceContainer& terminalInterfaces,
//								std::vector< std::vector< std::string > >& networkAddr);
//
//	void ConnectDevicesLayer3(Ptr<Node>& l3device,
//								NodeContainer& connectingDevices,
//								PointToPointHelper& p2p,
//								Ipv4InterfaceContainer& allInterfaces,
//								Ipv4InterfaceContainer& terminalInterfaces,
//								std::vector< std::vector< std::string > >& networkAddr);
	/**
	 * \brief Connects devices with layer 3 connection
	 * \param l3device The switch/router to which the nodes will connect
	 * \param connectingDevices The nodes to be connected to the switch
	 * \param p2p The configured point-to-point link
	 * \param networkAddress Network Subnet network address
	 * \param subnet Netowrk Subnet's subnet mask.
	 */
	void ConnectDevicesLayer3(Ptr<Node>& l3device,
								NodeContainer& connectingDevices,
								PointToPointHelper& p2p,
								std::string networkAddress, std::string subnet);
	/**
	 * \brief Connects two Layer 3 Devices together
	 * \param firstL3device The first switch/router in the connection
	 * \param secL3device The second switch/router in the connection
	 */
	void ConnectDevicesLayer3(Ptr<Node>& firstL3device,
								Ptr<Node> & secL3device,
								PointToPointHelper& p2p,
								std::string networkAddress, std::string subnet);
	/**
	 * \brief Connects devices with layer 3 connection and also send out the node interface container
	 * \param l3device The switch/router to which the nodes will connect
	 * \param connectingDevices The nodes to be connected to the switch
	 * \param p2p The configured point-to-point link
	 * \param terminalInterfaces The Node's IP interface container
	 * \param networkAddress Network Subnet network address
	 * \param subnet Netowrk Subnet's subnet mask.
	 */
	void ConnectDevicesLayer3(Ptr<Node>& l3device,
								NodeContainer& connectingDevices,
								PointToPointHelper& p2p,
								Ipv4InterfaceContainer& terminalInterfaces,
								std::string networkAddress, std::string subnet);
	/**
	 * \brief Connects devices with layer 3 connection and also send out the node interface container as well as
	 * all interfaces i.e. all interfaces of router/switch and all node's interfaces.
	 * \param l3device The switch/router to which the nodes will connect
	 * \param connectingDevices The nodes to be connected to the switch
	 * \param p2p The configured point-to-point link
	 * \param allInterfaces All IP interface container includes, all interfaces of router/switch and all node's interfaces
	 * \param terminalInterfaces The Node's IP interface container
	 * \param networkAddress Network Subnet network address
	 * \param subnet Netowrk Subnet's subnet mask.
	 */
	void ConnectDevicesLayer3(Ptr<Node>& l3device,
								NodeContainer& connectingDevices,
								PointToPointHelper& p2p,
								Ipv4InterfaceContainer& allInterfaces,
								Ipv4InterfaceContainer& terminalInterfaces,
								std::string networkAddress, std::string subnet);


private:
	/**
	 * \brief Connects devices with layer 2 channel and also send out the node interface container, the bridge created
	 * NetDevices on the bridge, and terminal NetDevices as well.
	 * \param l2switch The switch to which the nodes will connect
	 * \param connectingDevices The nodes to be connected to the switch
	 * \param csma The configured CSMA link
	 * \param bridge The bridge to be used
	 * \param bridgeDevices The bridge NetDevice container
	 * \param terminalDevices The Nodes NetDevice container
	 * \param terminalInterfaces The Node's IP interface container
	 * \param networkAddress Network Subnet network address
	 * \param subnet Netowrk Subnet's subnet mask.
	 */
	void CreateL2Connections(Ptr<Node>& l2switch,
								NodeContainer& connectingDevices,
								CsmaHelper& csma,
								BridgeHelper& bridge,
								NetDeviceContainer& bridgeDevices,
								NetDeviceContainer& terminalDevices,
								Ipv4InterfaceContainer& terminalInterfaces,
								std::string networkAddress, std::string subnet);

	/**
	 * \brief Connects devices with layer 3 connection and also send out the node interface container as well as
	 * all interfaces i.e. all interfaces of router/switch and all node's interfaces.
	 * \param l3device The switch/router to which the nodes will connect
	 * \param connectingDevices The nodes to be connected to the switch
	 * \param p2p The configured point-to-point link
	 * \param allInterfaces All IP interface container includes, all interfaces of router/switch and all node's interfaces
	 * \param terminalInterfaces The Node's IP interface container
	 * \param networkAddress Network Subnet network address
	 * \param subnet Netowrk Subnet's subnet mask.
	 */
	void CreateL3Connections(Ptr<Node>& l3device,
								NodeContainer& connectingDevices,
								PointToPointHelper& p2p,
								Ipv4InterfaceContainer& allInterfaces,
								Ipv4InterfaceContainer& terminalInterfaces,
								std::string networkAddress, std::string subnet);
};

} /* namespace ns3 */

#endif /* SCRATCH_NUTSHELL_CONNECTION_HELPER_H_ */
