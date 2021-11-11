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
 * node-utilization.h
 *
 *  Created on: Jan 25, 2017
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#ifndef SCRATCH_NUTSHELL_NODE_UTILIZATION_H
#define SCRATCH_NUTSHELL_NODE_UTILIZATION_H

#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"

#include "processing-power-util.h"
#include "storage-util.h"

namespace ns3 {

class NodeUtilization : public Object{
public:
	/**
	* \brief Get the type ID.
	* \return the object TypeId
	*/
	static TypeId GetTypeId(void);
	/**
	 * \brief Class constructor
	 */
	NodeUtilization();
	/**
	 * \brief Sets the Node ID for reference
	 * \param id the node ID, a uint32_t value
	 */
	void SetNodeId(uint32_t id);
	/**
	 * \brief Sets the base resources of node
	 *
	 * \param p the processing power
	 * \param ps the primary storage
	 * \param ss the secondary storage
	 */
	void SetBaseResources(ProcessingPower p, Storage ps, Storage ss);
	/**
	 * \brief Reserves resources and increase the amount of utilization
	 * \param p the processing power
	 * \param ps the priamry storage
	 * \param ss the secondary storage
	 */
	void ReserveResources(ProcessingPower p, Storage ps, Storage ss);
	/**
	 * \brief Releases resources and decrease the amount of utilization
	 */
	void ReleaseResources(ProcessingPower p, Storage ps, Storage ss);

	virtual ~NodeUtilization();


private:
	/**
	 * \brief Calculate the percentage of the provided numbers
	 * \param numerator the numerator value
	 * \param denominator the denominator value
	 * \return the percentage of given values
	 */
	double CalculatePercentage(uint64_t numerator, uint64_t denominator);

	void UpdateUtilization();

	uint32_t m_nodeId; //!< Stores the node ID.

	ProcessingPower m_processing; //!< Stores the base processing power of node
	Storage m_primary; //!< Stores the base primary storage of node
	Storage m_secondary; //!< Stores the base secondary storage of node

	ProcessingPower m_utilizedPower;  //!< Stores utilized processing power
	Storage m_utilizedPrimary; //!< Stores utilized primary storage
	Storage m_utilizedSecondary; //!< Stores utilized secondary storage


	TracedCallback<double, double, double, uint32_t> m_utilized;


};

} /* namespace ns3 */

#endif /* SCRATCH_NUTSHELL_NODE_UTILIZATION_H_ */
