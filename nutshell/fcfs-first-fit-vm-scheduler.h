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
 * fcfs-first-fit-vm-scheduler.h
 *
 *  Created on: Dec 26, 2015
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#ifndef NUTSHELL_FCFS_FIRST_FIT_VM_SCHEDULER
#define NUTSHELL_FCFS_FIRST_FIT_VM_SCHEDULER

#include "vm-scheduler.h"

#include <vector>

#include "ns3/ipv4-interface-container.h"
#include "ns3/node-container.h"


#include "processing-power-util.h"
#include "storage-util.h"
#include "application-size-util.h"
#include "datacenter-config.h"

#include "virtual-machine-helper.h"
#include "computational-node-container.h"

namespace ns3 {

class FcfsFirstFitVmScheduler: public VmScheduler {
public:
	/**
	* \brief Get the type ID.
	* \return the object TypeId
	*/
	static TypeId GetTypeId (void);
	/**
	 * \brief Class constructor
	 */
	FcfsFirstFitVmScheduler();
	/**
	 * \brief Class constructor to create and configure object
	 *
	 * The class accepts a configuration object and configure itself
	 * according to properties defined in the provided configuration object
	 *
	 * \param config The configuration, of DatacenterConfig type
	 */
	FcfsFirstFitVmScheduler(DatacenterConfig config);

	virtual ~FcfsFirstFitVmScheduler();
};

} /* namespace ns3 */

#endif /* NUTSHELL_FCFS_FIRST_FIT_VM_SCHEDULER */
