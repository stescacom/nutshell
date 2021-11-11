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
 * node-data.cc
 *
 *  Created on: Jan 25, 2017
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#include <vector>
#include <iostream>
#include <fstream>
#include <ctime>

#include "ns3/core-module.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/nstime.h"


#include "node-data.h"

namespace ns3 {

NodeData::NodeData()
	: NutshellDataCollector()
{
}

void
NodeData::EnableIndividualNodeData(bool w)
{
	m_isExportIndividualNodeData = w;
}

NodeData::~NodeData()
{
}

void
NodeData::AttachTraceSink()
{
//	NS_LOG_UNCOND("Attaching sink: " <<&NodeData::UtilizationSink);
	Config::ConnectWithoutContext("/NodeList/*/$ns3::NodeUtilization/UtilizedResources", MakeCallback(&NodeData::UtilizationSink, this));

	MapUtilization();
}

void
NodeData::ExportData()
{
	std::fstream file;
	std::string fileName = MakeFileName("avg-node-utilization-data", ".csv");
	fileName = StringConcat(m_simulationPrefix, fileName);
	file.open(fileName.c_str(), std::fstream::out);

	file << "Time,Utilized Nodes, Average Processing Power, Average Primary Storage, Average Secondary Storage\n";
	for(uint32_t i = 0; i < m_avgUtilization.size(); i++)
	{
		file << m_avgUtilization[i].seconds << "," << m_avgUtilization[i].total_nodes << ","
				<< m_avgUtilization[i].utilization.processing<< "," << m_avgUtilization[i].utilization.primaryStorage << ","
				<< m_avgUtilization[i].utilization.secondaryStorage << "\n";
	}
	file.close();
	if(m_isExportIndividualNodeData)
	{
		std::map<uint32_t, std::vector<MappedUtilization_s> >::iterator it;
		for(it = m_nodeUtilization.begin(); it != m_nodeUtilization.end(); it++)
		{
			std::fstream f;
			std::string fname = MakeFileName(it->first, ".csv");
			fname = StringConcat(m_simulationPrefix, fname);
			f.open(fname.c_str(), std::fstream::out);

			f << "Time, Node ID, Processing Power, Primary Storage, Secondary Storage\n";

			std::vector<MappedUtilization_s> wnu = it->second;
			for(uint32_t i = 0; i < wnu.size(); i++) {
				MappedUtilization_s mnu = wnu[i];

				f << mnu.seconds << "," << it->first << "," << mnu.utilization.processing << ","
						<< mnu.utilization.primaryStorage << "," << mnu.utilization.secondaryStorage << "\n";

			}

			f.close();
		}

	}

}



void
NodeData::UtilizationSink(double p, double ps, double ss, uint32_t nodeId)
{

	// preparing the record
	double t = Simulator::Now().GetSeconds();
	NodeUtilization_s nu = {
			p, ps, ss
	};

	MappedUtilization_s utilization = {
			t, nu
	};

	m_nodeUtilization[nodeId].push_back(utilization);
	m_currentNodeUtilization[nodeId] = nu;

}

void
NodeData::MapUtilization()
{
//	NS_LOG_UNCOND("Map Utilization Called" << Simulator::Now().GetSeconds());

	double t = Simulator::Now().GetSeconds();
	double sumP = 0;
	double sumPs = 0;
	double sumSs = 0;
	uint32_t count = 0;

	// calculate average and mapp it.
	std::map<uint32_t, NodeUtilization_s >::iterator it;
	for(it = m_currentNodeUtilization.begin(); it != m_currentNodeUtilization.end(); it++)
	{
		NodeUtilization_s nu = it->second;
		if(nu.primaryStorage != 0 && nu.secondaryStorage != 0 && nu.processing != 0)
		{
			sumP += nu.processing;
			sumPs += nu.primaryStorage;
			sumSs += nu.secondaryStorage;
			count++;
		}
	}

	if(count > 0)
	{
		NodeUtilization_s avgNu;

		avgNu.processing = sumP/count;
		avgNu.primaryStorage = sumPs/count;
		avgNu.secondaryStorage = sumSs/count;

		AvgUtilization_s au = {
				count, t, avgNu
		};
		m_avgUtilization.push_back(au);
	}



	Simulator::Schedule(Seconds(m_timeInterval), &NodeData::MapUtilization, this);


}

} /* namespace ns3 */
