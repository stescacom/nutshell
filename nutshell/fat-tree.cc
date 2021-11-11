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
 * fat-tree.cc
 *
 *  Created on: Nov 2, 2015
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#include <vector>
#include <math.h>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"

#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/node-container.h"

#include "fat-tree-config.h"
#include "fat-tree-addressing-scheme.h"
#include "access-network.h"
#include "computational-node-container.h"

#include "fat-tree.h"

namespace ns3 {

FatTree::FatTree(FatTreeConfig config)
{
	m_config = config;
	CreateArchitecture();
}

FatTree::~FatTree()
{
}

void
FatTree::CreateArchitecture()
{
	/*
	 * First of we calculate number of hosts and
	 * pod switches
	 */
	m_podBy2 = m_config.GetPods() / 2;
	m_rstart = 0;
	m_rend = m_rstart + m_podBy2;

	// Step 1: Generate all Addresses
	GenerateAddresses();

	// Step 2: Create all pods
	CreatePods();

	// step 3: Create core pod
	CreateCorePod();

	// step 4: connect core to pod's agg
	ConnectCoreToPod();

	// step 5: populate default routing table if no routing helper is set
	if(m_config.GetRouting() == NULL)
	{
		Ipv4GlobalRoutingHelper::PopulateRoutingTables();
	}

	// step 6: add traces if enabled
	NS_LOG_UNCOND("Tracing Status:" <<m_config.IsTracingEnabled());
	if(m_config.IsTracingEnabled())
	{
		m_config.GetDataCollector().AttachSinks();
//		NS_LOG_UNCOND(m_config.GetDataCollector());
	}

	//step 7 schedule VM
	BeginScheduling();


	// step 6: test connection
	//AddTestUdpEcho();

}

void
FatTree::GenerateAddresses()
{
	m_addrScheme = m_config.GetAddressingScheme();
	m_addrScheme->CreateNetworkAddresses();
	m_addrCount = 0;
//	for(uint32_t i = 0; i < m_addrScheme->GetNetworkList().size(); i++)
//	{
//		NS_LOG_UNCOND(i << "\t|" << m_addrScheme->GetNetwork(i) << "\t|" << m_addrScheme->GetSubnet(i));
//	}
}

void
FatTree::CreatePods()
{
	// sub step 1: resizing vectors according to pods
	m_aggSw.resize(m_config.GetPods());
	m_accessNet.resize(m_config.GetPods());

	// sub step 2: calculating total number of nodes and creating them
	CreateAllNodes();


	for(uint32_t i = 0; i < m_config.GetPods(); i++)
	{
		// sub step 3: create pod access networks
		CreateAccessNetwork(i);

		// sub step 4: create agg sw
		CreateAggSw(i);

		// sub step 5: connect pod agg sw to access net sw
		ConnectAggToAcc(i);
	}
}

void
FatTree::CreateAllNodes()
{
	// Set parameters
	m_allNodes.SetConfigRangeStrings(m_config.GetNodeMinProcessing(),
										m_config.GetNodeMaxProcessing(),
										m_config.GetNodeMinPrimaryStorage(),
										m_config.GetNodeMaxPrimaryStorage(),
										m_config.GetNodeMinSecondaryStorage(),
										m_config.GetNodeMaxSecondaryStorage());
	// Calculating total number of computational nodes
	uint32_t totNode = (pow(m_config.GetPods(), 3))/4;

	NS_LOG_UNCOND("Total Nodes: " << totNode);

	m_allNodes.Create(totNode);

}

void
FatTree::CreateAccessNetwork(uint32_t podIndex)
{
	std::vector<AccessNetwork> tmp;
	PointToPointHelper p2p = m_config.GetLink();
	InternetStackHelper stack = m_config.GetStack();

	for(uint32_t i = 0; i < m_podBy2; i++)
	{
		NodeContainer nodes;
		nodes.Add(m_allNodes.GetNodeContainerOfRange(m_rstart, m_rend));
		stack.Install(nodes);
		AccessNetwork an;
		an.SetNetwork(m_addrScheme->GetNetwork(m_addrCount), m_addrScheme->GetSubnet(m_addrCount));

		an.SetChannel(p2p);
		an.SetIntenetStack(stack);
		an.SetNodes(nodes);
		an.CreateNetwork();
		m_nodeInterfaces.Add(an.GetTerminalInterfaces());
		for(uint32_t j = m_rstart; j < m_rend; j++)
		{
			DataRateValue dv;
			m_allNodes.Get(j)->GetDevice(1)->GetAttribute("DataRate", dv);
			m_allNodes.Get(j)->SetNicDataRate(dv.Get());
		}
		tmp.push_back(an);
		m_addrCount++;
		m_rstart = m_rend;
		m_rend = m_rstart + m_podBy2;
	}
	m_accessNet[podIndex] = tmp;
}

void
FatTree::CreateAggSw(uint32_t podIndex)
{
	m_aggSw[podIndex].Create(m_podBy2);
	m_config.GetStack().Install(m_aggSw[podIndex]);
}

void
FatTree::CreateCorePod()
{
	uint32_t coreSwitches = pow(m_podBy2, 2);
	m_coreSw.Create(coreSwitches);
	m_config.GetStack().Install(m_coreSw);
}

void
FatTree::ConnectAggToAcc(uint32_t podIndex)
{
	ConnectionHelper ch;
	NodeContainer accSw;
	PointToPointHelper p2p = m_config.GetLink();
	for(uint32_t n = 0; n < m_podBy2; n++)
	{
		accSw.Add(m_accessNet[podIndex][n].GetAccessSwitch());
	}
	for(uint32_t i = 0; i < m_podBy2; i++)
	{
		Ptr<Node> aggSw = m_aggSw[podIndex].Get(i);
		ch.ConnectDevicesLayer3(aggSw, accSw, p2p, m_addrScheme->GetNetwork(m_addrCount), m_addrScheme->GetSubnet(m_addrCount));
		m_addrCount++;
	}
}
void
FatTree::ConnectCoreToPod()
{
	uint32_t aggCounter = 0;
	ConnectionHelper ch;
	PointToPointHelper p2p = m_config.GetLink();
	NodeContainer connectingAggSw;

	for(uint32_t i = 0; i < m_coreSw.GetN(); i++)
	{
		if((i > 0) && (i%m_podBy2 == 0))
		{
			aggCounter++;
		}
		if(i% m_podBy2 == 0)
		{
			NodeContainer tmpC;
			for(uint32_t j = 0; j < m_config.GetPods(); j++)
			{
				tmpC.Add(m_aggSw[j].Get(aggCounter));
			}
			connectingAggSw = tmpC;
		}
		Ptr<Node> coreSw = m_coreSw.Get(i);
		ch.ConnectDevicesLayer3(coreSw, connectingAggSw, p2p, m_addrScheme->GetNetwork(m_addrCount), m_addrScheme->GetSubnet(m_addrCount));
		m_addrCount++;
	}
}

//void
//FatTree::AddTestUdpEcho()
//{
//	uint32_t client = 0;
//	uint32_t serv = 52;
//	UdpEchoServerHelper echoServer (9);
//
//	ApplicationContainer serverApps = echoServer.Install (m_allNodes.Get (serv));
//	serverApps.Start (Seconds (1.0));
//	serverApps.Stop (Seconds (14.0));
//
//
//	Ptr<Node> question = m_allNodes.Get(serv);
//	Ptr<Ipv4> ipv4obj = question->GetObject<Ipv4>();
////	Ptr<Ipv4Address> ipv4add = ipv4obj->GetAddress(0, 0).GetLocal();
//
////	NS_LOG_UNCOND("Address of server device " << ipv4obj->GetAddress(1, 0).GetLocal());
////	NS_LOG_UNCOND("Address of server device in interfaces " << m_nodeInterfaces.GetAddress (serv));
////	NS_LOG_UNCOND("Address of client device in interfaces " << m_nodeInterfaces.GetAddress (client));
//
//	NS_LOG_UNCOND("ID of access net sw: " << m_accessNet[0][0].GetAccessSwitch()->GetId());
//
//	UdpEchoClientHelper echoClient (m_nodeInterfaces.GetAddress (serv), 9);
//	echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
//	echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
//	echoClient.SetAttribute ("PacketSize", UintegerValue (1024));
//
//	ApplicationContainer clientApps = echoClient.Install (m_allNodes.Get (client));
//	clientApps.Start (Seconds (2.0));
//	clientApps.Stop (Seconds (12.0));
//}

void
FatTree::BeginScheduling()
{

	m_config.GetVmScheduler()->SetConfiguration(m_config);
	m_config.GetVmScheduler()->SetNodes(m_allNodes);
	m_config.GetVmScheduler()->SetInterfaces(m_nodeInterfaces);
	m_config.GetVmScheduler()->BeginScheduling();

}

} /* namespace ns3 */
