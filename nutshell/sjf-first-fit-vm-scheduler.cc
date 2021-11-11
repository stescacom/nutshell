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
 * sjf-first-fit-vm-scheduler.cc
 *
 *  Created on: Mar 1, 2017
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#include <vector>
#include <stdlib.h>
#include <ctime>

#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/random-variable-stream.h"
#include "ns3/enum.h"
#include "ns3/double.h"
#include "ns3/uinteger.h"
#include "ns3/data-rate.h"

#include "processing-power-util.h"
#include "storage-util.h"
#include "application-size-util.h"
#include "datacenter-config.h"
#include "virtual-machine.h"
#include "network-vm.h"
#include "computational-local-data-vm.h"
#include "producer-vm.h"
#include "consumer-producer-vm.h"
#include "consumer-vm.h"
#include "virtual-machine-helper.h"
#include "computational-node-container.h"
#include "storage-server.h"
#include "vm-container.h"

#include "sjf-first-fit-vm-scheduler.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("SjfFirstFitVmScheduler");


NS_OBJECT_ENSURE_REGISTERED (SjfFirstFitVmScheduler);

TypeId SjfFirstFitVmScheduler::GetTypeId ()
{
	static TypeId tid = TypeId("ns3::SjfFirstFitVmScheduler")
		.SetParent<VmScheduler> ()
		.AddConstructor<SjfFirstFitVmScheduler> ();
	return tid;
}

SjfFirstFitVmScheduler::SjfFirstFitVmScheduler() {
	// TODO Auto-generated constructor stub

}

SjfFirstFitVmScheduler::SjfFirstFitVmScheduler(DatacenterConfig config) {
	m_config = config;
}

SjfFirstFitVmScheduler::~SjfFirstFitVmScheduler() {
	// TODO Auto-generated destructor stub
}

void
SjfFirstFitVmScheduler::SortVmList()
{
	NS_LOG_INFO(this);
	NS_LOG_UNCOND("Sorting SJF");
		for(uint32_t i = 0; i < m_vmList.size(); i++)
		{
			VmProperties tmp;
			for(uint32_t j = i + 1; j < m_vmList.size(); j++)
			{
				if(m_vmList[j].arrivalTime <= m_vmList[i].arrivalTime)
				{
					if(m_vmList[j].arrivalTime == m_vmList[i].arrivalTime)
					{
						uint32_t i_vmScore = 0;
						uint32_t j_vmScore = 0;
						if(m_vmList[i].dataAmount <= m_vmList[j].dataAmount)
						{
							i_vmScore += 10;
							j_vmScore += 5;
						}
						else
						{
							i_vmScore += 5;
							j_vmScore += 10;
						}

						if(m_vmList[i].appSize <= m_vmList[j].appSize)
						{
							i_vmScore += 10;
							j_vmScore += 5;
						}
						else
						{
							i_vmScore += 5;
							j_vmScore += 10;
						}

						if(m_vmList[i].processing < m_vmList[j].processing)
						{
							i_vmScore += 5;
							j_vmScore += 10;
						}
						else
						{
							i_vmScore += 10;
							j_vmScore += 5;
						}
						// greater score = shortest job
						if(i_vmScore < j_vmScore)
						{
							// swap i <-> j
							tmp = m_vmList[i];
							m_vmList[i] = m_vmList[j];
							m_vmList[j] = tmp;
						}
					}
					else
					{
						tmp = m_vmList[i];
						m_vmList[i] = m_vmList[j];
						m_vmList[j] = tmp;
					}

				}
			}
		}
}



} /* namespace ns3 */
