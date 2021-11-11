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
 * three-tier.cc
 *
 *  Created on: Oct 22, 2015
 *      Author: ubaid
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/ipv4-routing-helper.h"
#include "ns3/udp-echo-helper.h"
//#include "ns3/"

#include "addressing-scheme.h"
#include "computational-node-container.h"
#include "access-network.h"
#include "connection-helper.h"

#include "three-tier.h"

namespace ns3 {

ThreeTier::ThreeTier(ThreeTierConfig & config)
{
	m_config = config;
	m_ipsCovered = 0;
	CreateArchitecture();
}

ThreeTier::~ThreeTier()
{
}

void
ThreeTier::CreateArchitecture()
{
//	Config::SetDefault("ns3::Ipv4GlobalRouting::RandomEcmpRouting", BooleanValue(true));
	// step 1: creating all computational nodes
	CreateComputatinalNodes();

	// step 2 : Creating network address required
	GenerateNetworks();

	// step 3: Create All access networks
	CreateAccessNetwork();

	// step 4: Create Aggregate and core switches.
	CreateAggregateSwitches();
	CreateCoreSwitches();

	// step 5: connecting aggregate to access switches
	ConnectAggToAcc();

	// step 6: connect aggregate to neighboring agg in pod

	ConnectAggToAgg();

	// step 7: connect core to aggregate switches

	ConnectCoreToAgg();

	//step 8: connect traces to computational nodes if enabled
	NS_LOG_UNCOND("Tracing Status:" <<m_config.IsTracingEnabled());
	if(m_config.IsTracingEnabled())
	{
		m_config.GetDataCollector().AttachSinks();
//		NS_LOG_UNCOND(m_config.GetDataCollector());
	}

	// step 9: create and schedule VMs
	BeginScheduling();
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();
}

void
ThreeTier::CreateComputatinalNodes()
{
	// Set parameters
	m_totalNodes.SetConfigRangeStrings(m_config.GetNodeMinProcessing(),
										m_config.GetNodeMaxProcessing(),
										m_config.GetNodeMinPrimaryStorage(),
										m_config.GetNodeMaxPrimaryStorage(),
										m_config.GetNodeMinSecondaryStorage(),
										m_config.GetNodeMaxSecondaryStorage());

	// Calculating total number of computational nodes
	uint32_t totNode = m_config.GetNumOfAccessSwitches() * m_config.GetNodesPerAccessSwitch();
	NS_LOG_UNCOND("Total Nodes: " << totNode);

	m_totalNodes.Create(totNode);

}

void
ThreeTier::GenerateNetworks()
{
	/*
	 * Formula for total networks in
	 * ThreeTier
	 * We divide total agg and acc in pods
	 * pAgg = AggSwT / pods
	 * pAcc = AccSw / pods
	 * for core - agg: coreSw * AggSw
	 * for agg - agg: pods * (2*pAgg - 2)
	 * for agg - acc: pods * (pAgg * pAcc)
	 * for acc: AccSw
	 * Complete:
	 * (coreSw * AggSw) + (pods * (2*pAgg -2)) + (pods* (pAgg * pAcc) + AccSw)
	 */

	// dividing agg switches in pods
	uint32_t pAgg = m_config.GetNumOfAggSwitches()/m_config.GetNumOfPods();
	// dividing acc switches in pods
	uint32_t pAcc = m_config.GetNumOfAccessSwitches()/m_config.GetNumOfPods();

	// core to agg
	//uint32_t cTagg = m_config.GetNumOfCoreSwitches() * m_config.GetNumOfAggSwitches();


	// agg === agg
	uint32_t aggTagg = m_config.GetNumOfPods() * ((2 * pAgg) - 2);

	// agg - acc

	uint32_t aggTacc = m_config.GetNumOfPods() * (pAgg * pAcc);

	/*
	 * Now we have to set jumps, meaning
	 * calculate base networks for each access network
	 * access network will itself increment for each server link
	 * base network for agg to access, agg to agg, and core to agg
	 */
	uint32_t totalNet = m_config.GetNumOfCoreSwitches() + aggTagg + m_config.GetNumOfAggSwitches() + m_config.GetNumOfAccessSwitches();
	std::vector<uint16_t> jumps(totalNet);
	uint32_t jc = 0;
	// jumps for access

	// defining limits
	uint16_t first = m_config.GetNumOfAccessSwitches();
	uint16_t second = first + m_config.GetNumOfAggSwitches();
	uint16_t third = second + aggTagg;
	uint16_t fourth = third + m_config.GetNumOfCoreSwitches();

	for(uint32_t i = 0; i < totalNet; i++)
	{
		if(i < first)
		{
			jumps[jc] = m_config.GetNodesPerAccessSwitch();
		}
		else if( i >= first && i < second)
		{
			jumps[jc] = pAcc;
		}
		else if( i >= second && i < third)
		{
			jumps[jc] = 0;
		}
		else if ( i >= third && i < fourth)
		{
			jumps[jc] = m_config.GetNumOfAggSwitches();
		}

		jc++;

	}
	// jump for agg to agg
	jumps[jc] = aggTacc;


	NS_LOG_UNCOND("Total Networks " << totalNet);
	m_addrScheme = m_config.GetAddressingScheme();
	m_addrScheme->SetNetworkAddress(m_config.GetBaseNetworkAddr(), m_config.GetBaseSubnet());
	m_addrScheme->SetComponentNumber(255, totalNet);
	m_addrScheme->SetJumpValues(jumps);
	m_addrScheme->CreateNetworkAddresses();
}

void
ThreeTier::CreateAccessNetwork()
{
	m_accessNetworks.resize(m_config.GetNumOfAccessSwitches());
	InternetStackHelper st = m_config.GetInternetStack();
	PointToPointHelper p2p = m_config.GetNodeToAccLink();


	uint32_t start = 0;
	uint32_t end = start + m_config.GetNodesPerAccessSwitch();
	for(uint32_t i = 0; i < m_config.GetNumOfAccessSwitches(); i++)
	{
		NodeContainer accNodes = m_totalNodes.GetNodeContainerOfRange(start, end);
		m_allNodesForApps.Add(accNodes);
		st.Install(accNodes);
		m_accessNetworks[i].SetIntenetStack(st);
		m_accessNetworks[i].SetChannel(p2p);
		m_accessNetworks[i].SetNetwork(m_addrScheme->GetNetwork(i), m_addrScheme->GetSubnet(i));
		m_accessNetworks[i].SetNodes(accNodes);
		m_accessNetworks[i].CreateNetwork();
		m_nodesInterfaces.Add(m_accessNetworks[i].GetTerminalInterfaces());
		for(uint32_t j = start; j < end; j++)
		{
			DataRateValue dv;
			m_totalNodes.Get(j)->GetDevice(1)->GetAttribute("DataRate", dv);
			m_totalNodes.Get(j)->SetNicDataRate(dv.Get());
		}
		start = end;
		end = start + m_config.GetNodesPerAccessSwitch();
		m_ipsCovered++;
	}
}

void
ThreeTier::CreateAggregateSwitches()
{
	m_aggSwitches.Create(m_config.GetNumOfAggSwitches());
	m_config.GetInternetStack().Install(m_aggSwitches);
}
void
ThreeTier::CreateCoreSwitches()
{
	m_coreSwitches.Create(m_config.GetNumOfCoreSwitches());
	m_config.GetInternetStack().Install(m_coreSwitches);
}

std::string
ThreeTier::GetNetworkAddressFor(uint32_t n)
{
	return m_addrScheme->GetNetwork(n);
}
std::string
ThreeTier::GetSubnetMaskFor(uint32_t n)
{
	return m_addrScheme->GetSubnet(n);
}
void
ThreeTier::ConnectAggToAcc()
{
	uint32_t accNetPerPod = m_config.GetNumOfAccessSwitches() / m_config.GetNumOfPods();
	uint32_t aggSw = m_config.GetNumOfAggSwitches();
	uint32_t aggPerPod = aggSw/m_config.GetNumOfPods();

	uint32_t podBegin = 0;
	uint32_t podEnd = podBegin + accNetPerPod;
	ConnectionHelper connect;
	for(uint32_t i = 0; i < aggSw; i++)
	{
		Ptr<Node> aggSwitch = m_aggSwitches.Get(i);
		NodeContainer accSwitches;

		PointToPointHelper p2pAccAgg = m_config.GetAccToAggLink();

		for(uint32_t j = podBegin; j < podEnd; j++)
		{
			accSwitches.Add(m_accessNetworks[j].GetAccessSwitch());
		}

		// now connecting the selected agg switch to all access switches of the pod
//		NS_LOG_UNCOND(m_addrScheme->GetNetwork(m_ipsCovered) << "Ip Coverd: " << m_ipsCovered);
		connect.ConnectDevicesLayer3(aggSwitch, accSwitches, p2pAccAgg,
										m_addrScheme->GetNetwork(m_ipsCovered),
										m_addrScheme->GetSubnet(m_ipsCovered));

		m_ipsCovered++;
//		NS_LOG_UNCOND("i = " << i << " | Pod Begin: " << podBegin << " | Pod End: " << podEnd);
		if((i+1)%aggPerPod == 0)
		{
			podBegin = podEnd;
			podEnd = podBegin + accNetPerPod;
//			NS_LOG_UNCOND("Condition True: \ni = " << i << " | Pod Begin: " << podBegin << " | Pod End: " << podEnd);
		}
	}
}
void
ThreeTier::ConnectAggToAgg()
{
	uint32_t pAgg = m_config.GetNumOfAggSwitches()/ m_config.GetNumOfPods();
	uint32_t aggCount = 0;
	PointToPointHelper aggTaggP2p = m_config.GetAggToCoreLink();
	ConnectionHelper ch;
	for(uint32_t i = 0; i < m_config.GetNumOfPods(); i++)
	{
		for(uint32_t j = 0; j < (pAgg - 1); j++)
		{
//			NS_LOG_UNCOND("Connecting Agg Switch: " << aggCount << " to " << aggCount+1 << " with Networks:\n");
			Ptr<Node> currentAggSw = m_aggSwitches.Get(aggCount);
			Ptr<Node> nextAggSw = m_aggSwitches.Get(aggCount+1);
			// establishing a forward link
			ch.ConnectDevicesLayer3(currentAggSw, nextAggSw, aggTaggP2p,
									m_addrScheme->GetNetwork(m_ipsCovered),
									m_addrScheme->GetSubnet(m_ipsCovered));
//			NS_LOG_UNCOND("Forward: " << m_addrScheme.GetNetwork(m_ipsCovered));
			m_ipsCovered++;

			// establishing a backward link

			ch.ConnectDevicesLayer3(nextAggSw, currentAggSw, aggTaggP2p,
									m_addrScheme->GetNetwork(m_ipsCovered),
									m_addrScheme->GetSubnet(m_ipsCovered));

			m_ipsCovered++;

			// incrementing aggregate switch counter
			aggCount++;
		}
		aggCount++;
	}
}
void
ThreeTier::ConnectCoreToAgg()
{
	PointToPointHelper aggTcore = m_config.GetAggToCoreLink();
	ConnectionHelper ch;

	for(uint32_t i = 0; i < m_config.GetNumOfCoreSwitches(); i++)
	{
		Ptr<Node> curCore = m_coreSwitches.Get(i);
		ch.ConnectDevicesLayer3(curCore, m_aggSwitches, aggTcore,
								m_addrScheme->GetNetwork(m_ipsCovered),
								m_addrScheme->GetSubnet(m_ipsCovered));
		m_ipsCovered++;
	}
}

void
ThreeTier::AddTestUdpEcho()
{
	uint32_t client = 230;
	uint32_t serv = 0;
	UdpEchoServerHelper echoServer (9);

	ApplicationContainer serverApps = echoServer.Install (m_allNodesForApps.Get (serv));
	serverApps.Start (Seconds (1.0));
//	serverApps.Stop (Seconds (10.0));


	Ptr<Node> question = m_allNodesForApps.Get(serv);
	Ptr<Ipv4> ipv4obj = question->GetObject<Ipv4>();
//	Ptr<Ipv4Address> ipv4add = ipv4obj->GetAddress(0, 0).GetLocal();

	NS_LOG_UNCOND("Address of server device " << ipv4obj->GetAddress(1, 0).GetLocal());
	NS_LOG_UNCOND("Address of server device in interfaces " << m_nodesInterfaces.GetAddress (serv));
	NS_LOG_UNCOND("Address of client device in interfaces " << m_nodesInterfaces.GetAddress (client));

	UdpEchoClientHelper echoClient (m_nodesInterfaces.GetAddress (serv), 9);
	echoClient.SetAttribute ("MaxPackets", UintegerValue (2));
	echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
	echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

	ApplicationContainer clientApps = echoClient.Install (m_allNodesForApps.Get (client));
	clientApps.Start (Seconds (2.0));
//	clientApps.Stop (Seconds (10.0));
}

void
ThreeTier::BeginScheduling()
{
	m_config.GetVmScheduler()->SetConfiguration(m_config);
	m_config.GetVmScheduler()->SetNodes(m_totalNodes);
	m_config.GetVmScheduler()->SetInterfaces(m_nodesInterfaces);
	m_config.GetVmScheduler()->BeginScheduling();
}

} /* namespace ns3 */
