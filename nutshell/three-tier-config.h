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
 * three-tier-config.h
 *
 *  Created on: Oct 19, 2015
 *      Author: ubaid
 */

#ifndef THREE_TIER_CONFIG
#define THREE_TIER_CONFIG

#include "ns3/data-rate.h"
#include "ns3/ipv4-routing-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/point-to-point-helper.h"

#include "datacenter-config.h"
#include "addressing-scheme.h"
#include "vm-scheduler.h"

namespace ns3 {

class ThreeTierConfig: public DatacenterConfig {
public:
	ThreeTierConfig();

	void SetPods(const uint32_t pods);
	void SetSwitchesNumber(const uint32_t accSwNo,
							const uint32_t aggSwNo,
							const uint32_t coreSwNo);

	void SetNodesPerAccessSwitch(const uint32_t nodesPerAccess);

	void SetLinks(const PointToPointHelper &nodeToAcc,
					const PointToPointHelper &accToAgg,
					const PointToPointHelper &aggToCore);

	void SetNetwork(const std::string networkAddr,
					const std::string subnet);

	void SetInternetStack(const InternetStackHelper &stack);
	void SetRoutingHelper(Ipv4RoutingHelper &custRouting);
	void SetAddressingScheme(AddressingScheme &addrScheme, std::string network, std::string subnet);
	void SetVmScheuler(VmScheduler & sch);

	uint32_t GetNumOfPods() const;
	uint32_t GetNodesPerAccessSwitch() const;
	uint32_t GetNumOfAccessSwitches() const;
	uint32_t GetNumOfAggSwitches() const;
	uint32_t GetNumOfCoreSwitches() const;

	PointToPointHelper GetNodeToAccLink() const;
	PointToPointHelper GetAccToAggLink() const;
	PointToPointHelper GetAggToCoreLink() const;

	InternetStackHelper GetInternetStack() const;

	AddressingScheme * GetAddressingScheme() const;
	VmScheduler * GetVmScheduler() const;

	std::string GetBaseNetworkAddr() const;
	std::string GetBaseSubnet() const;

	virtual ~ThreeTierConfig();
private:
	uint32_t m_nodesPerAccSwitch,
			m_accSwitches,
			m_aggSwitches,
			m_coreSwitches,
			m_pods;
	std::string m_baseNetowrkAddress, m_baseSubnet;
	PointToPointHelper m_nodeToAcc, m_accToAgg, m_aggToCore;
	InternetStackHelper m_stack;
	Ipv4RoutingHelper * m_customRouting;
	AddressingScheme * m_addressing;
	VmScheduler * m_customVmScheduler;

};

} /* namespace ns3 */

#endif /* THREE_TIER_CONFIG */
