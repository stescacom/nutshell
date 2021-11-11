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
 * queue-data.h
 *
 *  Created on: Jan 25, 2017
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#ifndef SCRATCH_NUTSHELL_DATA_COLLECTOR_QUEUE_DATA_H_
#define SCRATCH_NUTSHELL_DATA_COLLECTOR_QUEUE_DATA_H_

#include <vector>

#include "nutshell-data-collector.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"

namespace ns3 {

class QueueData : public NutshellDataCollector {
public:
	/**
	 * \brief Creates an object to collect queue data
	 */
	QueueData();

	/**
	 * \brief Enables queue packet drop trace
	 *
	 * Enabling packet drop trace for queue will export a .pcap file if set to true along
	 * with other information.
	 * \param pcap The flag value, true if require a .pcap file
	 */
	void EnableQueueTracePacketDrop(bool pcap = false);
	/**
	 * \brief Enables tracing of dequeue of a net device queue
	 *
	 * Enabling the dequeue will export a .pcap file if set to true along
	 * with other information.
	 *
	 */
	void EnableQueueTraceDequeue(bool pcap = false);
	/**
	 * \brief Enables tracing of enqueue of a net device queue
	 *
	 * Enabling the enqueue will export a .pcap file if set to true along
	 * with other information.
	 *
	 */
	void EnableQueueTraceEnqueue(bool pcap = false);

	/**
	 * \brief Attach a trace sink to trace source
	 *
	 * This method enable the class to attach different trace sinks to
	 * trace source provided by different NS-3 classes.
	 */
	virtual void AttachTraceSink();

	/**
	 * \brief Exports collected data to CSV format.
	 *
	 * This method exports data to a CSV format, defined by each
	 * child class.
	 */
	virtual void ExportData();
	/**
	 * \brief Set the target queue connection string
	 */
	void SetTargetConnection(std::string con);

	virtual ~QueueData();
private:
	struct CounterMap_s {
		double time;
		uint64_t count;
	};
	EventId m_mapEvent; //!< Keep track of scheduled event
	bool m_enqueuePcap; //!< Flag to enable or disable .pacp for packet enqueue
	bool m_dequeuePcap; //!< Flag to enable or disable .pcap for packet dequeue
	bool m_dropPcap; //!< Flag to enable or disable .pcap for packet drop

	bool m_enqueue; //!< Flag to enable or disable data collection for packet enqueue
	bool m_dequeue; //!< Flag to enable or disable data collection for packet dequeue
	bool m_drop; //!< Flag to enable or disable data collection for packet drop

	std::string m_conString; //!< Connection string to a queue of NetDevice

	uint64_t m_totalPacketDrop; //!< Packet drop counter
	uint64_t m_totalPacketEnqueue; //!< Packet enqueue counter
	uint64_t m_totalPacketDequeue; //!< Packet dequeue counter


	 Ptr<PcapFileWrapper> m_packetDrop; //!< Packet drop pcap writer
	 Ptr<PcapFileWrapper> m_packetEnqueue; //!< Packet enqueue pcap writer
	 Ptr<PcapFileWrapper> m_packetDequeue; //!< Packet dequeue pcap writer

	 std::vector<CounterMap_s> m_packetDropMap; //!< Holds counter history for packet drop
	 std::vector<CounterMap_s> m_packetEnqueueMap; //!< Holds counter history for packet enqueue
	 std::vector<CounterMap_s> m_packetDequeueMap; //!< Holds counter history for packet dequeue


	/**
	 * \brief Callback to catch the traced packet drop
	 * \param p The packet dropped by queue
	 */
	void PacketDropSink(Ptr<const Packet> p);
	/**
	 * \brief Callback to catch the enqueued packet
	 * \param p The packet enqueued
	 */
	void EnqueueSink(Ptr<const Packet> p);
	/**
	 * \brief Callback to catch the enqueued packet
	 * \param p The packet dequeued
	 */
	void DequeueSink(Ptr<const Packet> p);

	/**
	 * \brief Maps the packet drop history
	 */
	void MapPacketDrop();
	/**
	 * \brief Maps the packet enqueue history
	 */
	void MapPacketEnqueue();
	/**
	 * \brief Maps the packet dequeue history
	 */
	void MapPacketDequeue();

	/**
	 * \brief Export Packet drop data
	 */
	void ExportPacketDrop();
	/**
	 * \brief Export Packet enqueue data
	 */
	void ExportPacketEnqueue();
	/**
	 * \brief Export Packet dequeue data
	 */
	void ExportPacketDequeue();

};

} /* namespace ns3 */

#endif /* SCRATCH_NUTSHELL_DATA_COLLECTOR_QUEUE_DATA_H_ */
