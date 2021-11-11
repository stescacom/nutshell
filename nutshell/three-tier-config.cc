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
 * three-tier-config.cc
 *
 *  Created on: Oct 19, 2015
 *      Author: ubaid
 */

#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"

#include "three-tier-config.h"
#include "vm-scheduler.h"
#include "fcfs-first-fit-vm-scheduler.h"

namespace ns3 {

ThreeTierConfig::ThreeTierConfig()
{
	m_addressing = new AddressingScheme();
	m_customRouting = NULL;
	m_customVmScheduler = new FcfsFirstFitVmScheduler();
}

void
ThreeTierConfig::SetPods(const uint32_t pods)
{
	m_pods = pods;
}

void
ThreeTierConfig::SetSwitchesNumber(const uint32_t accSwNo,
									const uint32_t aggSwNo,
									const uint32_t coreSwNo)
{
	m_accSwitches = accSwNo;
	m_aggSwitches = aggSwNo;
	m_coreSwitches = coreSwNo;
}
void
ThreeTierConfig::SetNodesPerAccessSwitch(const uint32_t nodesPerAccess)
{
	m_nodesPerAccSwitch = nodesPerAccess;
}
void
ThreeTierConfig::SetLinks(const PointToPointHelper &nodeToAcc,
							const PointToPointHelper &accToAgg,
							const PointToPointHelper &aggToCore)
{
	m_nodeToAcc = nodeToAcc;
	m_accToAgg = accToAgg;
	m_aggToCore = aggToCore;
}
void
ThreeTierConfig::SetNetwork(const std::string networkAddr,
					const std::string subnet)
{
	m_baseNetowrkAddress = networkAddr;
	m_baseSubnet = subnet;
}

void
ThreeTierConfig::SetInternetStack(const InternetStackHelper &stack)
{
	m_stack = stack;
}

void
ThreeTierConfig::SetRoutingHelper(Ipv4RoutingHelper &custRouting)
{
	m_customRouting = &custRouting;
	Ipv4ListRoutingHelper list;
	Ipv4StaticRoutingHelper staticRouting;
	list.Add(staticRouting, 0);
	list.Add(custRouting, -10);
	m_stack.SetRoutingHelper(list);
}
void
ThreeTierConfig::SetAddressingScheme(AddressingScheme & addrScheme, std::string network, std::string subnet)
{
	m_addressing = &addrScheme;
	SetNetwork(network, subnet);
}

void
ThreeTierConfig::SetVmScheuler(VmScheduler & sch)
{
	m_customVmScheduler = &sch;
}

uint32_t
ThreeTierConfig::GetNumOfPods() const
{
	return m_pods;
}
uint32_t
ThreeTierConfig::GetNodesPerAccessSwitch() const
{
	return m_nodesPerAccSwitch;
}
uint32_t
ThreeTierConfig::GetNumOfAccessSwitches() const
{
	return m_accSwitches;
}
uint32_t
ThreeTierConfig::GetNumOfAggSwitches() const
{
	return m_aggSwitches;
}
uint32_t
ThreeTierConfig::GetNumOfCoreSwitches() const
{
	return m_coreSwitches;
}
PointToPointHelper
ThreeTierConfig::GetNodeToAccLink() const
{
	return m_nodeToAcc;
}
PointToPointHelper
ThreeTierConfig::GetAccToAggLink() const
{
	return m_accToAgg;
}
PointToPointHelper
ThreeTierConfig::GetAggToCoreLink() const
{
	return m_aggToCore;
}

InternetStackHelper
ThreeTierConfig::GetInternetStack() const
{
	return m_stack;
}

AddressingScheme *
ThreeTierConfig::GetAddressingScheme() const
{
	return m_addressing;
}

std::string
ThreeTierConfig::GetBaseNetworkAddr() const
{
	return m_baseNetowrkAddress;
}
std::string
ThreeTierConfig::GetBaseSubnet() const
{
	return m_baseSubnet;
}

VmScheduler *
ThreeTierConfig::GetVmScheduler() const
{
	return m_customVmScheduler;
}

ThreeTierConfig::~ThreeTierConfig()
{
}

} /* namespace ns3 */
