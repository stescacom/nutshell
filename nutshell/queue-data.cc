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
 * queue-data.cc
 *
 *  Created on: Jan 25, 2017
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#include <fstream>
#include <vector>
#include "queue-data.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

namespace ns3 {

QueueData::QueueData()
	: NutshellDataCollector(), m_mapEvent ()
{
	m_enqueue = false;
	m_enqueuePcap = false;

	m_dequeue = false;
	m_dequeuePcap = false;

	m_drop = false;
	m_dropPcap = false;

	m_totalPacketDrop = 0;
	m_totalPacketEnqueue = 0;
	m_totalPacketDequeue = 0;
}

QueueData::~QueueData()
{
}

void
QueueData::EnableQueueTracePacketDrop(bool pcap)
{
	if(pcap)
	{
		m_dropPcap = true;
		PcapHelper pcapHelper;
		std::string fileName = MakeFileName("queue_packet_drop", ".pcap");
		fileName = StringConcat(m_simulationPrefix, fileName);
		m_packetDrop = pcapHelper.CreateFile (fileName, std::ios::out, PcapHelper::DLT_PPP);
	}
	m_drop = true;
}

void
QueueData::EnableQueueTraceDequeue(bool pcap)
{
	if(pcap)
	{
		m_dequeuePcap = true;
		PcapHelper pcapHelper;
		std::string fileName = MakeFileName("queue_dequeue_packet", ".pcap");
		fileName = StringConcat(m_simulationPrefix, fileName);
		m_packetDequeue = pcapHelper.CreateFile (fileName, std::ios::out, PcapHelper::DLT_PPP);
	}
	m_dequeue = true;
}

void
QueueData::EnableQueueTraceEnqueue(bool pcap)
{
	if(pcap)
	{
		m_enqueuePcap = true;
		PcapHelper pcapHelper;
		std::string fileName = MakeFileName("queue_enqueue_packet", ".pcap");
		fileName = StringConcat(m_simulationPrefix, fileName);
		m_packetEnqueue = pcapHelper.CreateFile (fileName, std::ios::out, PcapHelper::DLT_PPP);
	}
	m_enqueue = true;
}

void
QueueData::AttachTraceSink()
{
	if(m_drop)
	{
		Config::ConnectWithoutContext(StringConcat(m_conString, "/Drop"), MakeCallback(&QueueData::PacketDropSink, this));
		MapPacketDrop();
	}

	if(m_enqueue)
	{
		Config::ConnectWithoutContext(StringConcat(m_conString, "/Enqueue"), MakeCallback(&QueueData::PacketDropSink, this));
		MapPacketEnqueue();
	}

	if(m_dequeue)
	{
		Config::ConnectWithoutContext(StringConcat(m_conString, "/Dequeue"), MakeCallback(&QueueData::PacketDropSink, this));
		MapPacketDequeue();
	}

}

void
QueueData::ExportData()
{
	if(m_drop)
	{
		ExportPacketDrop();
	}
	if(m_enqueue)
	{
		ExportPacketEnqueue();
	}
	if(m_dequeue)
	{
		ExportPacketDequeue();
	}
}

void
QueueData::ExportPacketDrop()
{
	std::fstream file;
	std::string fileName = MakeFileName("packet_drop_hisory", ".csv");
	fileName = StringConcat(m_simulationPrefix, fileName);
	file.open(fileName.c_str(), std::fstream::out);

	file << "Time,Total Packet Drop\n";
	for(uint32_t i = 0; i < m_packetDropMap.size(); i++)
	{

		file << m_packetDropMap[i].time << "," << m_packetDropMap[i].count << "\n";
	}
	file.close();
}

void
QueueData::ExportPacketEnqueue()
{
	std::fstream file;
	std::string fileName = MakeFileName("packet_enqueue_hisory", ".csv");
	fileName = StringConcat(m_simulationPrefix, fileName);
	file.open(fileName.c_str(), std::fstream::out);

	file << "Time,Total Packet Enqueue\n";
	for(uint32_t i = 0; i < m_packetEnqueueMap.size(); i++)
	{

		file << m_packetEnqueueMap[i].time << "," << m_packetEnqueueMap[i].count << "\n";
	}
	file.close();
}

void
QueueData::ExportPacketDequeue()
{
	std::fstream file;
	std::string fileName = MakeFileName("packet_dequeue_hisory", ".csv");
	fileName = StringConcat(m_simulationPrefix, fileName);
	file.open(fileName.c_str(), std::fstream::out);

	file << "Time,Total Packet Dequeue\n";
	for(uint32_t i = 0; i < m_packetDequeueMap.size(); i++)
	{

		file << m_packetDequeueMap[i].time << "," << m_packetDequeueMap[i].count << "\n";
	}
	file.close();
}

void
QueueData::SetTargetConnection(std::string con)
{
	m_conString = con;
}

void
QueueData::PacketDropSink(Ptr<const Packet> p)
{
	m_totalPacketDrop++;
	if(m_dropPcap)
	{
		m_packetDrop->Write(Simulator::Now(), p);
	}
	MapPacketDrop();

}

void
QueueData::EnqueueSink(Ptr<const Packet> p)
{
	m_totalPacketEnqueue++;
	if(m_enqueuePcap)
	{
		m_packetEnqueue->Write(Simulator::Now(), p);
	}
	MapPacketEnqueue();
}

void
QueueData::DequeueSink(Ptr<const Packet> p)
{
	m_totalPacketEnqueue++;
	if(m_enqueuePcap)
	{
		m_packetEnqueue->Write(Simulator::Now(), p);
	}
	MapPacketDequeue();
}

void
QueueData::MapPacketDrop()
{
	CounterMap_s c = {
			Simulator::Now().GetSeconds(), m_totalPacketDrop
	};
	m_packetDropMap.push_back(c);
	Simulator::Schedule(Seconds(m_timeInterval), &QueueData::MapPacketDrop, this);
}

void
QueueData::MapPacketEnqueue()
{
	CounterMap_s c = {
			Simulator::Now().GetSeconds(), m_totalPacketEnqueue
	};
	m_packetEnqueueMap.push_back(c);
	Simulator::Schedule(Seconds(m_timeInterval), &QueueData::MapPacketEnqueue, this);
}

void
QueueData::MapPacketDequeue()
{
	CounterMap_s c = {
			Simulator::Now().GetSeconds(), m_totalPacketDequeue
	};
	m_packetDequeueMap.push_back(c);
	Simulator::Schedule(Seconds(m_timeInterval), &QueueData::MapPacketDequeue, this);
}

} /* namespace ns3 */
