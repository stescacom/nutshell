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
 * fcfs-first-fit-vm-scheduler.cc
 *
 *  Created on: Dec 26, 2015
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

#include "fcfs-first-fit-vm-scheduler.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("FcfsFirstFitVmScheduler");


NS_OBJECT_ENSURE_REGISTERED (FcfsFirstFitVmScheduler);

TypeId FcfsFirstFitVmScheduler::GetTypeId ()
{
	static TypeId tid = TypeId("ns3::FcfsFirstFitVmScheduler")
		.SetParent<VmScheduler> ()
		.AddConstructor<FcfsFirstFitVmScheduler> ();
	return tid;
}

FcfsFirstFitVmScheduler::FcfsFirstFitVmScheduler()
{
}

FcfsFirstFitVmScheduler::FcfsFirstFitVmScheduler(DatacenterConfig config)
{
	m_config = config;
}



FcfsFirstFitVmScheduler::~FcfsFirstFitVmScheduler()
{
	// TODO Auto-generated destructor stub
}

} /* namespace ns3 */
