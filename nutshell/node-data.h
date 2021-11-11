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
 * node-data.h
 *
 *  Created on: Jan 25, 2017
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#ifndef NUTSHELL_DATA_COLLECTOR_NODE_DATA_H
#define NUTSHELL_DATA_COLLECTOR_NODE_DATA_H

#include <vector>
#include <map>

#include "nutshell-data-collector.h"
#include "ns3/nstime.h"
#include "ns3/core-module.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

namespace ns3 {


/**
 * \brief Collects data for a node
 *
 * This data collector collects data from traces defined
 * on a computational node.
 */
class NodeData: public NutshellDataCollector {

public:
	/**
	 * \brief Creates an object to collect node data
	 */
	NodeData();

	// node utilization
	void EnableIndividualNodeData(bool w);

	virtual ~NodeData();

private:
	/**
	 * \brief structure to hold utilization percentage for a single node
	 */
	struct NodeUtilization_s {
		double processing, primaryStorage, secondaryStorage;
	};

	/**
	 * \brief Structure to map time to average utilization for a single node
	 */
	struct MappedUtilization_s {
		double seconds;
		NodeUtilization_s utilization;
	};

	struct AvgUtilization_s {
		uint32_t total_nodes;
		double seconds;
		NodeUtilization_s utilization;
	};

	/**
	 * \brief Trace sink for computational node utilization
	 *
	 * \param p the value of processing power
	 * \param ps the value of primary storage
	 * \param ss the value of secondary storage
	 * \param nodeId the id of computational node
	 */
	void UtilizationSink(double p, double ps, double ss, uint32_t nodeId);

	/**
	 * \brief Maps current utilization after an interval to time
	 *
	 * The function is called after an interval defined, the current
	 * state of Computational Nodes is dumped, and an average utilization
	 * is calculated, also the number of Nodes currently under utilization are
	 * counted and traced.
	 */
	void MapUtilization();
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


	std::map<uint32_t, std::vector<MappedUtilization_s> > m_nodeUtilization;
	std::map<uint32_t, NodeUtilization_s > m_currentNodeUtilization;
	std::vector<AvgUtilization_s> m_avgUtilization;
	bool m_isExportIndividualNodeData;
};

} /* namespace ns3 */

#endif /* SCRATCH_NUTSHELL_DATA_COLLECTOR_NODE_DATA_H_ */
