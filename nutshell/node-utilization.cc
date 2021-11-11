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
 * node-utilization.cc
 *
 *  Created on: Jan 25, 2017
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#include "node-utilization.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"

#include "processing-power-util.h"
#include "storage-util.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NodeUtilization");

NS_OBJECT_ENSURE_REGISTERED (NodeUtilization);


TypeId
NodeUtilization::GetTypeId ()
{
	static TypeId tid = TypeId("ns3::NodeUtilization")
		.SetParent (Object::GetTypeId ())
		.AddConstructor<NodeUtilization> ()
		.AddTraceSource ("UtilizedResources",
						   "Trace for utilization",
						   MakeTraceSourceAccessor (&NodeUtilization::m_utilized))
		;
	return tid;
}

NodeUtilization::NodeUtilization()
{
}

NodeUtilization::~NodeUtilization()
{
	// TODO Auto-generated destructor stub
}

void
NodeUtilization::SetNodeId(uint32_t id)
{
	m_nodeId = id;

}

void
NodeUtilization::SetBaseResources(ProcessingPower p, Storage ps, Storage ss)
{
	m_processing = p;
	m_primary = ps;
	m_secondary = ss;

	m_utilizedPower = m_processing - m_processing;
	m_utilizedPrimary = m_primary - m_primary;
	m_utilizedSecondary = m_secondary - m_secondary;

	UpdateUtilization();
}

void
NodeUtilization::ReserveResources(ProcessingPower p, Storage ps, Storage ss)
{

	m_utilizedPower = m_utilizedPower + p;
	m_utilizedPrimary = m_utilizedPrimary + ps;
	m_utilizedSecondary = m_utilizedSecondary + ss;

	UpdateUtilization();
}

void
NodeUtilization::ReleaseResources(ProcessingPower p, Storage ps, Storage ss)
{

	m_utilizedPower = m_utilizedPower - p;
	m_utilizedPrimary = m_utilizedPrimary - ps;
	m_utilizedSecondary = m_utilizedSecondary - ss;

	UpdateUtilization();
}

double
NodeUtilization::CalculatePercentage(uint64_t numerator, uint64_t denominator)
{

	if(denominator != 0)
	{
		double n = (double)numerator;
		double d = (double)denominator;
		double res = n/d;
		return (res*100);
	}
	else
	{
		return 0;
	}
}

void
NodeUtilization::UpdateUtilization()
{
	double p = CalculatePercentage(m_utilizedPower.GetProcessingPower(), m_processing.GetProcessingPower());
	double ps = CalculatePercentage(m_utilizedPrimary.GetStorage(), m_primary.GetStorage());
	double ss = CalculatePercentage(m_utilizedSecondary.GetStorage(), m_secondary.GetStorage());

	m_utilized (p, ps, ss, m_nodeId);

}

} /* namespace ns3 */
