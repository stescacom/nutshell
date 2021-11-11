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
 * three-tier.h
 *
 *  Created on: Oct 22, 2015
 *      Author: ubaid
 */

#ifndef THREE_TIER
#define THREE_TIER

#include <vector>

#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/node-container.h"
#include "ns3/ipv4-interface-container.h"

#include "three-tier-config.h"
#include "computational-node-container.h"
#include "access-network.h"
#include "connection-helper.h"


namespace ns3 {

class ThreeTier {
public:
	ThreeTier(ThreeTierConfig &config);
	virtual ~ThreeTier();
private:
	void CreateArchitecture();
	void CreateComputatinalNodes();
	void CreateAggregateSwitches();
	void CreateCoreSwitches();
	void CreateAccessNetwork();
	void GenerateNetworks();

	std::string GetNetworkAddressFor(uint32_t n);
	std::string GetSubnetMaskFor(uint32_t n);
	void ConnectAggToAcc();
	void ConnectAggToAgg();
	void ConnectCoreToAgg();

	void AddTestUdpEcho();
	void BeginScheduling();

	ThreeTierConfig m_config;
	ComputationalNodeContainer m_totalNodes;
	NodeContainer m_allNodesForApps;
	NodeContainer m_coreSwitches, m_aggSwitches;
	Ipv4InterfaceContainer m_nodesInterfaces;
	PointToPointHelper m_nodeToAccP2p, m_accToAggP2p, m_aggToAccP2p;
	AddressingScheme* m_addrScheme;
	uint32_t m_ipsCovered;

	std::vector<AccessNetwork> m_accessNetworks;
	std::vector<Ipv4InterfaceContainer> m_aggInterfaces;
	std::vector<Ipv4InterfaceContainer> m_coreInterfaces;


};

} /* namespace ns3 */

#endif /* THREE_TIER */
