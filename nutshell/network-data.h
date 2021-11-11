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
 * network-data.h
 *
 *  Created on: Feb 9, 2017
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#ifndef SCRATCH_NUTSHELL_NETWORK_DATA_H_
#define SCRATCH_NUTSHELL_NETWORK_DATA_H_

#include <vector>
#include "ns3/core-module.h"
#include "ns3/flow-monitor-module.h"
#include "nutshell-data-collector.h"

namespace ns3 {

class NetworkData: public NutshellDataCollector {
public:
	NetworkData();
	virtual ~NetworkData();
	/**
	 * \brief Process network traffic data
	 *
	 */
	void ProcessTraffic();
private:
	/**
	 * \brief Structure to hold a single record of network
	 * statistics
	 */
	struct NetworkStat_s {
		double time;
		uint32_t numOfFlows;

		uint32_t avgTxPacket;
		uint32_t avgRxPacket;

		uint64_t avgTxBytes;
		uint64_t avgRxBytes;

		uint32_t avgDropPacket;
		uint32_t avgLostPacket;

		double avgLostPacketRatio;
		double avgPacketDeliveryRatio;

		double avgThroughput;
		double avgMeanDelay;
		double avgMeanJitter;
	};
	Ptr<FlowMonitor>    m_monitor; //!< Flow monitor
	FlowMonitorHelper m_flowmonHelper; //!< Flow monitor helper
	std::vector<NetworkStat_s> m_netStat; //!< History of network statistic
	uint32_t m_macTxDrop; //!< MAC Tx drop counter
	uint32_t m_phyTxDrop; //!< Physical layer Tx drop counter
	uint32_t m_phyRxDrop; //!< Physical layer Rx drop counter

	std::vector<FlowId> m_flows; //!< Keeps list of flow IDs


	uint32_t m_txPacketsum; //!< Sum of transmitted packets
	uint32_t m_rxPacketsum; //!< Sum of received packets
	uint32_t m_dropPacketsum; //!< Sum of dropped packets
	uint32_t m_lostPacketsum; //!< Sum of lost packet
	uint64_t m_txBytesSum; //!< Sum of transmitted bytes
	uint64_t m_rxBytesSum; //!< Sum of received bytes
	double m_lostPacketRatioSum; //!< Sum of lost packet ratio
	double m_packetDeliverySum; //!< Sum of packet delivered




	virtual void AttachTraceSink();
	virtual void ExportData();

	/**
	 * \brief Counts MAC transmitted packet drop
	 */
	void MacTxDrop(Ptr<const Packet> p);

	/**
	 * \brief Counts Physical layer transmitted packet drop
	 */
	void PhyTxDrop(Ptr<const Packet> p);

	/**
	 * \brief Counts Physical layer received packet drop
	 */
	void PhyRxDrop(Ptr<const Packet> p);
	/**
	 * \brief Checks if flow is counted
	 * \param id FlowId
	 * \return True if id exist, false otherwise.
	 */
	bool FlowIdExist(FlowId id);
	/**
	 * \brief Calculate average and add to list
	 */
	void CalculateAverage(uint32_t txPacketsum,
	uint32_t rxPacketsum,
	uint32_t dropPacketsum,
	uint32_t lostPacketsum,
	uint64_t txBytesSum,
	uint64_t rxBytesSum,
	double lostPacketRatioSum,
	double packetDeliverySum,
	double throughputSum,
	double meanDelaySum, double meanJitterSum, uint32_t flowCounter);
	/**
	 * \brief Export details of calculated stats
	 */
	void ExportDetails();
	/**
	 * \brief Export network stats summary
	 */
	void ExportSummary();
};

} /* namespace ns3 */

#endif /* SCRATCH_NUTSHELL_NETWORK_DATA_H_ */
