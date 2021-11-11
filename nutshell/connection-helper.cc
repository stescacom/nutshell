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
 * connection-helper.cc
 *
 *  Created on: Oct 6, 2015
 *      Author: ubaid
 */

#include <vector>

#include "ns3/node.h"
#include "ns3/pointer.h"
#include "ns3/node-container.h"
#include "ns3/csma-helper.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/log.h"

#include "connection-helper.h"


namespace ns3 {

void
ConnectionHelper::ConnectDevicesLayer2(Ptr<Node>& l2switch,
										NodeContainer& connectingDevices,
										CsmaHelper& csma,
										std::string networkAddress, std::string subnet)
{
	BridgeHelper bridge;
	NetDeviceContainer bridgeNetDevices;
	NetDeviceContainer terminalDevices;
	Ipv4InterfaceContainer interfaces;
	CreateL2Connections(l2switch, connectingDevices, csma, bridge, bridgeNetDevices, terminalDevices,
						interfaces, networkAddress, subnet);
}

void
ConnectionHelper::ConnectDevicesLayer2(Ptr<Node>& l2switch,
										NodeContainer& connectingDevices,
										CsmaHelper& csma,
										Ipv4InterfaceContainer& terminalInterfaces,
										std::string networkAddress, std::string subnet)
{
	BridgeHelper bridge;
	NetDeviceContainer bridgeNetDevices;
	NetDeviceContainer terminalDevices;
	CreateL2Connections(l2switch, connectingDevices, csma, bridge, bridgeNetDevices, terminalDevices,
							terminalInterfaces, networkAddress, subnet);
}

void
ConnectionHelper::ConnectDevicesLayer2(Ptr<Node>& l2switch,
										NodeContainer& connectingDevices,
										CsmaHelper& csma,
										BridgeHelper& bridge,
										NetDeviceContainer& bridgeDevices,
										NetDeviceContainer& terminalDevices,
										Ipv4InterfaceContainer& terminalInterfaces,
										std::string networkAddress, std::string subnet)
{
	CreateL2Connections(l2switch, connectingDevices, csma, bridge, bridgeDevices, terminalDevices,
								terminalInterfaces, networkAddress, subnet);
}
/*
void
ConnectionHelper::ConnectDevicesLayer3(Ptr<Node>& l3device,
										NodeContainer& connectingDevices,
										PointToPointHelper& p2p,
										std::vector< std::vector< std::string > >& networkAddr)
{

	Ipv4InterfaceContainer allInterfaces, terminalInterfaces;
	CreateL3Connections(l3device, connectingDevices, p2p, allInterfaces, terminalInterfaces, networkAddr);
}

void
ConnectionHelper::ConnectDevicesLayer3(Ptr<Node>& l3device,
										NodeContainer& connectingDevices,
										PointToPointHelper& p2p,
										Ipv4InterfaceContainer& terminalInterfaces,
										std::vector< std::vector< std::string > >& networkAddr)
{
	Ipv4InterfaceContainer allInterfaces;
	CreateL3Connections(l3device, connectingDevices, p2p, allInterfaces, terminalInterfaces, networkAddr);
}

void
ConnectionHelper::ConnectDevicesLayer3(Ptr<Node>& l3device,
										NodeContainer& connectingDevices,
										PointToPointHelper& p2p,
										Ipv4InterfaceContainer& allInterfaces,
										Ipv4InterfaceContainer& terminalInterfaces,
										std::vector< std::vector< std::string > >& networkAddr)
{

	CreateL3Connections(l3device, connectingDevices, p2p, allInterfaces, terminalInterfaces, networkAddr);
}
*/
void
ConnectionHelper::ConnectDevicesLayer3(Ptr<Node>& l3device,
										NodeContainer& connectingDevices,
										PointToPointHelper& p2p,
										std::string networkAddress, std::string subnet)
{
	Ipv4InterfaceContainer allInterfaces, terminalInterfaces;
	CreateL3Connections(l3device, connectingDevices, p2p, allInterfaces, terminalInterfaces, networkAddress, subnet);
//	int c = 0;
//	for(Ipv4InterfaceContainer::Iterator i = terminalInterfaces.Begin(); i != terminalInterfaces.End(); i++)
//	{
//		NS_LOG_UNCOND(c+1 << ") IP: " << terminalInterfaces.GetAddress(c));
//		c++;
//	}
}

void
ConnectionHelper::ConnectDevicesLayer3(Ptr<Node>& firstL3device,
										Ptr<Node> & secL3device,
										PointToPointHelper& p2p,
										std::string networkAddress, std::string subnet)
{
	NodeContainer connectingDevices;
	connectingDevices.Add(secL3device);
	Ipv4InterfaceContainer allInterfaces, terminalInterfaces;
	CreateL3Connections(firstL3device, connectingDevices, p2p, allInterfaces, terminalInterfaces, networkAddress, subnet);
//	int c = 0;
//	for(Ipv4InterfaceContainer::Iterator i = terminalInterfaces.Begin(); i != terminalInterfaces.End(); i++)
//	{
//		NS_LOG_UNCOND(c+1 << ") IP: " << terminalInterfaces.GetAddress(c));
//		c++;
//	}
}

void
ConnectionHelper::ConnectDevicesLayer3(Ptr<Node>& l3device,
										NodeContainer& connectingDevices,
										PointToPointHelper& p2p,
										Ipv4InterfaceContainer& terminalInterfaces,
										std::string networkAddress, std::string subnet)
{
	Ipv4InterfaceContainer allInterfaces;
	CreateL3Connections(l3device, connectingDevices, p2p, allInterfaces, terminalInterfaces, networkAddress, subnet);
}


void
ConnectionHelper::ConnectDevicesLayer3(Ptr<Node>& l3device,
										NodeContainer& connectingDevices,
										PointToPointHelper& p2p,
										Ipv4InterfaceContainer& allInterfaces,
										Ipv4InterfaceContainer& terminalInterfaces,
										std::string networkAddress, std::string subnet)
{
	CreateL3Connections(l3device, connectingDevices, p2p, allInterfaces, terminalInterfaces, networkAddress, subnet);
}

void
ConnectionHelper::CreateL2Connections(Ptr<Node>& l2switch,
										NodeContainer& connectingDevices,
										CsmaHelper& csma,
										BridgeHelper& bridge,
										NetDeviceContainer& bridgeDevices,
										NetDeviceContainer& terminalDevices,
										Ipv4InterfaceContainer& terminalInterfaces,
										std::string networkAddress, std::string subnet)
{
	for(NodeContainer::Iterator i = connectingDevices.Begin(); i != connectingDevices.End(); i++)
	{
		NodeContainer sub = NodeContainer(*i, l2switch);
		NetDeviceContainer link = csma.Install(sub);
		bridgeDevices.Add(link.Get(1));
		terminalDevices.Add(link.Get(0));
	}
	bridge.Install(l2switch, bridgeDevices);
	Ipv4AddressHelper ipv4;
	ipv4.SetBase(networkAddress.c_str(), subnet.c_str());
	terminalInterfaces = ipv4.Assign(terminalDevices);
}

void
ConnectionHelper::CreateL3Connections(Ptr<Node>& l3device,
										NodeContainer& connectingDevices,
										PointToPointHelper& p2p,
										Ipv4InterfaceContainer& allInterfaces,
										Ipv4InterfaceContainer& terminalInterfaces,
										std::string networkAddress, std::string subnet)
{
	Ipv4AddressHelper ipv4;
	ipv4.SetBase(networkAddress.c_str(), subnet.c_str());
	for(NodeContainer::Iterator i = connectingDevices.Begin(); i != connectingDevices.End(); i++)
	{
		NodeContainer sub = NodeContainer(*i, l3device);
		NetDeviceContainer link = p2p.Install(sub);
		Ipv4InterfaceContainer linkInterface = ipv4.Assign(link);
		allInterfaces.Add(linkInterface);
		terminalInterfaces.Add(linkInterface.Get(0));
		ipv4.NewNetwork();
	}
}

} /* namespace ns3 */
