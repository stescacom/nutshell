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
 * fat-tree-config.cc
 *
 *  Created on: Nov 2, 2015
 *      Author: ubaid
 */


#include "ns3/ipv4-routing-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"

#include "fat-tree-addressing-scheme.h"

#include "fat-tree-config.h"
#include "fat-tree-ipv4-routing-protocol.h"
#include "fat-tree-ipv4-routing-header.h"
#include "fat-tree-ipv4-routing-protocol-helper.h"
#include "fat-tree-ipv4-rte.h"
#include "vm-scheduler.h"
#include "fcfs-first-fit-vm-scheduler.h"

namespace ns3 {

FatTreeConfig::FatTreeConfig()
{
	m_addressing = NULL;
	m_customRouting = new FatTreeIpv4RoutingProtocolHelper();
//	m_customRouting = NULL;
	m_customVmScheduler = new FcfsFirstFitVmScheduler();
	Ipv4ListRoutingHelper list;
	Ipv4StaticRoutingHelper staticRouting;
	list.Add(staticRouting, 0);
	list.Add(*m_customRouting, -10);
	m_stack.SetRoutingHelper(list);
}

void
FatTreeConfig::SetPods(uint32_t pods)
{
	m_pods = pods;
	m_defaultAddressing.SetPods(pods);
}
void
FatTreeConfig::SetBaseNetwork(std::string netAddr, std::string subnet)
{
	m_baseNetowrkAddress = netAddr;
	m_baseSubnet = subnet;
	m_defaultAddressing.SetNetworkAddress(netAddr, subnet);
}
void
FatTreeConfig::SetLink(PointToPointHelper p2p)
{
	m_p2pLink = p2p;
}
void
FatTreeConfig::SetRouting(Ipv4RoutingHelper & customRouting)
{
	m_customRouting = &customRouting;
	Ipv4ListRoutingHelper list;
	Ipv4StaticRoutingHelper staticRouting;
	list.Add(staticRouting, 0);
	list.Add(customRouting, -10);
	m_stack.SetRoutingHelper(list);
}
void
FatTreeConfig::SetAddressingScheme(AddressingScheme & addressing)
{
	m_addressing = &addressing;
}

void
FatTreeConfig::SetInternetStack(InternetStackHelper internet)
{
	m_stack = internet;
}

uint32_t
FatTreeConfig::GetPods() const
{
	return m_pods;
}
PointToPointHelper
FatTreeConfig::GetLink() const
{
	return m_p2pLink;
}
Ipv4RoutingHelper *
FatTreeConfig::GetRouting() const
{
	return m_customRouting;
}
AddressingScheme *
FatTreeConfig::GetAddressingScheme()
{
	if(m_addressing == NULL)
	{
		m_defaultAddressing.SetNetworkAddress(GetNetworkAddress(), GetSubnet());
		m_defaultAddressing.SetPods(m_pods);
		m_addressing = &m_defaultAddressing;
	}
	return m_addressing;
}

void
FatTreeConfig::SetVmScheuler(VmScheduler & sch)
{
	m_customVmScheduler = &sch;
}

InternetStackHelper
FatTreeConfig::GetStack() const
{
	return m_stack;
}

std::string
FatTreeConfig::GetNetworkAddress() const
{
	return m_baseNetowrkAddress;
}
std::string
FatTreeConfig::GetSubnet() const
{
	return m_baseSubnet;
}

VmScheduler *
FatTreeConfig::GetVmScheduler() const
{
	return m_customVmScheduler;
}

FatTreeConfig::~FatTreeConfig()
{
}


} /* namespace ns3 */
