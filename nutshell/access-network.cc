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
 * access-network.cc
 *
 *  Created on: Oct 21, 2015
 *      Author: ubaid
 */

#include "ns3/object.h"
#include "ns3/node-container.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-interface-container.h"

#include "access-network.h"
#include "connection-helper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("AccessNetwork");

NS_OBJECT_ENSURE_REGISTERED (AccessNetwork);


TypeId
AccessNetwork::GetTypeId()
{
	static TypeId tid = TypeId("ns3::AccessNetwork")
						.SetParent<Object>()
						.AddConstructor<AccessNetwork>()
						;
	return tid;
}

AccessNetwork::AccessNetwork()
{
}

AccessNetwork::AccessNetwork(NodeContainer &nodes,
								PointToPointHelper &p2p,
								InternetStackHelper &stack,
								std::string networkAdd,
								std::string subnet)
{
	NS_LOG_FUNCTION(this);
	m_nodes = nodes;
	m_networkAddress = networkAdd;
	m_subnetMask = subnet;
	m_p2p = p2p;
	m_istack = stack;
	ConnectNodesToSwitch();
}

void
AccessNetwork::CreateNetwork()
{
	NS_LOG_FUNCTION(this);
	ConnectNodesToSwitch();
}

void
AccessNetwork::SetNodes(NodeContainer &nodes)
{
	NS_LOG_FUNCTION(this);
	m_nodes = nodes;
}
void
AccessNetwork::SetChannel(PointToPointHelper &p2p)
{
	NS_LOG_FUNCTION(this);
	m_p2p = p2p;
}
void
AccessNetwork::SetNetwork(std::string networkAdd, std::string subnet)
{
	NS_LOG_FUNCTION(this);
	m_networkAddress = networkAdd;
	m_subnetMask = subnet;
}

void
AccessNetwork::SetIntenetStack(InternetStackHelper &stack)
{
	NS_LOG_FUNCTION(this);
	m_istack = stack;
}

Ptr<Node>
AccessNetwork::GetAccessSwitch() const
{
	NS_LOG_FUNCTION(this);
	return m_accessSwitch;
}
std::string
AccessNetwork::GetNetworkAddress() const
{
	NS_LOG_FUNCTION(this);
	return m_networkAddress;
}
std::string
AccessNetwork::GetSubnet() const
{
	NS_LOG_FUNCTION(this);
	return m_subnetMask;
}

Ipv4InterfaceContainer
AccessNetwork::GetTerminalInterfaces() const
{
	NS_LOG_FUNCTION(this);
	return m_terminalDevices;
}
Ipv4InterfaceContainer
AccessNetwork::GetAllInterfaces() const
{
	NS_LOG_FUNCTION(this);
	return m_connectedDevices;
}

AccessNetwork::~AccessNetwork()
{
}

void
AccessNetwork::ConnectNodesToSwitch()
{
	NS_LOG_FUNCTION(this);
	NodeContainer sw;
	sw.Create(1);
	m_accessSwitch = sw.Get(0);
	m_istack.Install(m_accessSwitch);
	ConnectionHelper ch;
	ch.ConnectDevicesLayer3(m_accessSwitch, m_nodes, m_p2p, m_connectedDevices, m_terminalDevices, m_networkAddress, m_subnetMask);
}

} /* namespace ns3 */
