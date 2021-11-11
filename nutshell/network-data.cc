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
 * network-data.cc
 *
 *  Created on: Feb 9, 2017
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#include <vector>
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "nutshell-data-collector.h"
#include "network-data.h"

namespace ns3 {

NetworkData::NetworkData()
	: NutshellDataCollector()
{
	m_macTxDrop = 0;
	m_phyRxDrop = 0;
	m_phyTxDrop = 0;

	m_txPacketsum = 0;
	m_rxPacketsum = 0;
	m_dropPacketsum = 0;
	m_lostPacketsum = 0;
	m_txBytesSum = 0;
	m_rxBytesSum = 0;
	m_lostPacketRatioSum = 0;
	m_packetDeliverySum = 0;


}

NetworkData::~NetworkData()
{
}

void
NetworkData::AttachTraceSink()
{
	NS_LOG_UNCOND("Attaching sinks");

	m_monitor = m_flowmonHelper.InstallAll();
	NS_LOG_UNCOND("Monitor :" << m_monitor);
	Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Mac/MacTxDrop", MakeCallback(&NetworkData::MacTxDrop, this));
	Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyRxDrop", MakeCallback(&NetworkData::PhyRxDrop, this));
	Config::ConnectWithoutContext("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/PhyTxDrop", MakeCallback(&NetworkData::PhyTxDrop, this));
	Simulator::Schedule(Seconds(m_timeInterval), &NetworkData::ProcessTraffic, this);
}

void
NetworkData::ExportData()
{
	NS_LOG_UNCOND("Exporting network data");
//	ProcessTraffic();
//	ExportSummary();
	ExportDetails();
}

void
NetworkData::ExportDetails()
{
	std::fstream file;
	std::string fileName = MakeFileName("network_stats", ".csv");
	fileName = StringConcat(m_simulationPrefix, fileName);
	file.open(fileName.c_str(), std::fstream::out);

	file << "Total Mac Tx Drop," << m_macTxDrop << "\n";
	file << "Total Phy Tx Drop," << m_phyTxDrop << "\n";
	file << "Total Phy Rx Drop," << m_phyRxDrop << "\n\n";

	file << "Time, No. of Flows, Avg Tx Packet, Avg Rx Packet, Avg Tx Bytes, Avg Rx Bytes, Avg Drop Packets, "
			<< "Avg Lost Packet, Avg Packet Delivery Ratio (%), Avg Packet Lost Ratio (%), Avg Throughput (Mbps), Avg Mean Delay, Avg Mean Jitter\n";

	for(uint32_t i = 0; i < m_netStat.size(); i++)
	{
		file << m_netStat[i].time << "," << m_netStat[i].numOfFlows << ","
				<< m_netStat[i].avgTxPacket << "," << m_netStat[i].avgRxPacket << ","
				<< m_netStat[i].avgTxBytes << "," << m_netStat[i].avgRxBytes << "," << m_netStat[i].avgDropPacket << ","
				<< m_netStat[i].avgLostPacket << "," << m_netStat[i].avgPacketDeliveryRatio << ","
				<< m_netStat[i].avgLostPacketRatio << "," << m_netStat[i].avgThroughput << ","
				<< m_netStat[i].avgMeanDelay << "," << m_netStat[i].avgMeanJitter << "\n";
	}
	file.close();
}

void
NetworkData::ExportSummary()
{
//	std::fstream file;
//	std::string fileName = MakeFileName("network_stats_summary", ".txt");
//	file.open(fileName.c_str(), std::fstream::out);
//	file << "Total Flows:\t" << m_flows.size() << "\n";
//	file << "Total Tx Packets\t" << m_txPacketsum << "\n";
//	file << "Total Rx Packets\t" << m_rxPacketsum << "\n";
//	file << "Total Tx Bytes\t" << m_txBytesSum << "\n";
//	file << "Total Rx Bytes\t" << m_rxBytesSum << "\n";
//	file << "Total Drop Packets\t" << m_dropPacketsum << "\n";
//
//	file << "Total Lost Packets\t" << m_lostPacketsum << "\n";
//	file << "Total Lost Packet Ratio (%)\t" << (m_lostPacketRatioSum/m_flows.size()) << "\n";
//	file << "Total Delivered Packets Ratio(%)\t" << (m_packetDeliverySum / m_flows.size()) << "\n";
//	file << "Total Mac Tx Drop\t" << m_macTxDrop << "\n";
//	file << "Total Phy Tx Drop\t" << m_phyTxDrop << "\n";
//	file << "Total Phy Rx Drop\t" << m_phyRxDrop << "\n";
//
//	file.close();

}

void
NetworkData::ProcessTraffic()
{
	NS_LOG_UNCOND("Processing Traffic");
	Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier>(
			m_flowmonHelper.GetClassifier());

	std::map<FlowId, FlowMonitor::FlowStats> stats = m_monitor->GetFlowStats();

	uint32_t txPacketsum = 0;
	uint32_t rxPacketsum = 0;
	uint32_t dropPacketsum = 0;
	uint32_t lostPacketsum = 0;
	uint64_t txBytesSum = 0;
	uint64_t rxBytesSum = 0;
	double lostPacketRatioSum = 0;
	double packetDeliverySum = 0;

	double throughputSum = 0;
	double meanDelaySum = 0;
	double meanJitterSum = 0;

	uint32_t flowCounter = 0;

	for (std::map<FlowId, FlowMonitor::FlowStats>::iterator flow =
			stats.begin(); flow != stats.end(); flow++) {
//		NS_LOG_UNCOND("Flow exist");
		if(!FlowIdExist(flow->first))
		{
			m_flows.push_back(flow->first);
		}
		flowCounter++;


//		Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow(flow->first);

		txBytesSum += flow->second.txBytes;
		rxBytesSum += flow->second.rxBytes;
		txPacketsum += flow->second.txPackets;
		rxPacketsum += flow->second.rxPackets;
		lostPacketsum += flow->second.lostPackets;
		dropPacketsum += flow->second.packetsDropped.size();

//		lostPacketRatioSum += ((double) flow->second.txPackets - (double) flow->second.rxPackets) / (double) flow->second.txPackets;

		lostPacketRatioSum += ((lostPacketsum / txPacketsum) * 100);

		packetDeliverySum += ((rxPacketsum/txPacketsum) * 100);

		throughputSum += (flow->second.rxBytes * 8.0 / (flow->second.timeLastRxPacket.GetSeconds()-flow->second.timeFirstTxPacket.GetSeconds())/(1024*1024));

		meanDelaySum += (flow->second.delaySum.GetSeconds() / flow->second.rxPackets);

		meanJitterSum += (flow->second.jitterSum.GetSeconds()
						/ (flow->second.rxPackets));

	}

	// calculating averages

	CalculateAverage(txPacketsum,
			rxPacketsum,
			dropPacketsum,
			lostPacketsum,
			txBytesSum,
			rxBytesSum,
			lostPacketRatioSum,
			packetDeliverySum,
			throughputSum,
			meanDelaySum, meanJitterSum, flowCounter);

	Simulator::Schedule(Seconds(m_timeInterval), &NetworkData::ProcessTraffic, this);

}



void
NetworkData::MacTxDrop(Ptr<const Packet> p)
{
	NS_LOG_UNCOND("MAC Packet Drop");
	m_macTxDrop++;
}

void
NetworkData::PhyTxDrop(Ptr<const Packet> p)
{
	NS_LOG_UNCOND("PHY Packet Drop");
	m_phyTxDrop++;
}
void
NetworkData::PhyRxDrop(Ptr<const Packet> p)
{
	NS_LOG_UNCOND("PhyRx Packet Drop");
	m_phyRxDrop++;
}

bool
NetworkData::FlowIdExist(FlowId id)
{
	for(uint32_t i = 0; i < m_flows.size(); i++)
	{
		if(id == m_flows[i])
		{
			return true;
		}
	}
	return false;
}

void
NetworkData::CalculateAverage(uint32_t txPacketsum,
		uint32_t rxPacketsum,
		uint32_t dropPacketsum,
		uint32_t lostPacketsum,
		uint64_t txBytesSum,
		uint64_t rxBytesSum,
		double lostPacketRatioSum,
		double packetDeliverySum,
		double throughputSum,
		double meanDelaySum, double meanJitterSum, uint32_t flowCounter)
{
	NetworkStat_s s;
	if(flowCounter > 0)
	{
//		NS_LOG_UNCOND("Mean Delay Sum" << m_meanDelaySum);
		s.numOfFlows = flowCounter;
		s.avgDropPacket = meanDelaySum / s.numOfFlows;
		s.avgLostPacket = lostPacketsum / s.numOfFlows;
		s.avgLostPacketRatio = (lostPacketRatioSum / s.numOfFlows);
		s.avgMeanDelay = meanDelaySum / s.numOfFlows;
		s.avgMeanJitter = meanJitterSum / s.numOfFlows;
		s.avgPacketDeliveryRatio = (packetDeliverySum / s.numOfFlows);
		s.avgRxBytes = rxBytesSum / s.numOfFlows;
		s.avgTxBytes = txBytesSum / s.numOfFlows;
		s.avgRxPacket = rxPacketsum / s.numOfFlows;
		s.avgThroughput = throughputSum  / s.numOfFlows;
		s.avgTxPacket = txPacketsum / s.numOfFlows;
		s.time = Simulator::Now().GetSeconds();

		m_netStat.push_back(s);
	}


}

} /* namespace ns3 */
