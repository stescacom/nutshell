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
 * vm-scheduler.h
 *
 *  Created on: Oct 23, 2015
 *      Author: ubaid
 */

#ifndef NUTSHELL_VM_SCHEDULER_H
#define NUTSHELL_VM_SCHEDULER_H

#include <vector>
#include <map>

#include "ns3/object.h"
#include "ns3/callback.h"
#include "ns3/ptr.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/node-container.h"
#include "ns3/data-rate.h"


#include "processing-power-util.h"
#include "storage-util.h"
#include "application-size-util.h"
#include "datacenter-config.h"

#include "virtual-machine-helper.h"
#include "computational-node-container.h"

namespace ns3 {

enum DataSource_e {
		STORAGE_SERVER,
		LOCAL_DISK
	};

struct VmProperties {
		ProcessingPower 	processing;
		Storage 			primary, secondary;
		ApplicationSize 	appSize;
		Time 				arrivalTime;
		bool				requrieData;
		DataSource_e		dataSource;
		Storage				dataAmount;
		Storage				hddRwRate, memRwRate;
		uint32_t			numOfProcAccesses;
		Time				memAccessTime, hddAccessTime;
		double				memPDF;
		uint32_t			mtu;
		std::string			cProtocolTid;
		DataRate			transRate;
	};

struct ExecutedVm_s {
	double dispatched_time;
	uint32_t nid;
	VmProperties vm;
};

struct SplitExecutedVm_s {
	double dispatched_time;
	VmProperties actualVm;
	std::vector<ExecutedVm_s> splittedVm;
};

class VmScheduler : public Object {
public:
	/**
	* \brief Get the type ID.
	* \return the object TypeId
	*/
	static TypeId GetTypeId (void);
	VmScheduler();
	VmScheduler(DatacenterConfig config);
	void SetConfiguration(DatacenterConfig config);
	void SetNodes(ComputationalNodeContainer c);
	void SetInterfaces(Ipv4InterfaceContainer iface);
	/**
	 * \brief Begins the scheduling process
	 */
	virtual void BeginScheduling();
	virtual ~VmScheduler();

	uint32_t GetExecutedVmNum();
	uint32_t GetNotExecutedVmNum();

	std::vector<ExecutedVm_s> GetExecutedVmList();
	std::vector<SplitExecutedVm_s> GetSplitExecutedVmList();
	std::vector<VmProperties> GetNotExecutedVmList();

protected:

	DatacenterConfig 			m_config;
	std::vector<VmProperties> 	m_vmList;
	ComputationalNodeContainer 	m_dataCenterNodes;
	Ipv4InterfaceContainer		m_dcNodesInterfaces;

	ComputationalNodeContainer	m_storageServers;
	Ipv4InterfaceContainer		m_storageServersIfases;

	std::vector<uint32_t>		m_ratio;
	uint32_t					m_ratioSum;

	std::vector<uint32_t> m_portAssigned;

	/**
	 * \brief Creates a list of Virtual Machines, according to the configuration
	 */
	virtual void CreateVmList();
	/**
	 * \brief Creates storage servers from within the architecture
	 */
	virtual void CreateStorageServers();
	virtual void SortVmList();

	/**
	 * \brief Schedules a Virtual Machine
	 *
	 * \param vm The configuration of VM to create
	 */
	virtual void ScheduleVm(VmProperties & vm);
	/**
	 * \brief Dispatches a VM to computational node
	 *
	 * \param vm The configuration of VM to dispatch
	 */
	virtual void DispatchVmOnNode(VmProperties & vm);
	/**
	 * \brief Splits a VM if enabled, and start the execution
	 *
	 * \param vm The configuration of VM to split and start
	 */
	virtual bool SplitAndStart(VmProperties & vm);
	/**
	 * \brief Splits a VM
	 *
	 * \param vm The configuration of VM to split
	 * \param spvec split ratio
	 */
	virtual void SplitVm(VmProperties & vm, std::vector<VmProperties> & spvec);
	/**
	 * \brief Finds nodes for the splitted VMs
	 */
	virtual void FindNodeIndicies(std::vector<VmProperties> & vmSplitted,
			ComputationalNodeContainer & selectedNodes, Ipv4InterfaceContainer & selectedNodeIface);
	/**
	 * \brief Starts VM on its computational Node.
	 *
	 * \param splittedVmVec The vector containing splitted VM
	 * \param selectedNodes The list of nodes selected to send splitted VMs to
	 * \param selectedNodeIface The list of selected node IPs
	 * \param assigned A confirmation vector to confirm each portion is assigned to computational node.
	 */
	virtual void StartVm(std::vector<VmProperties> & splittedVmVec,
			ComputationalNodeContainer & selectedNodes, Ipv4InterfaceContainer & selectedNodeIface,
			std::vector<bool> & assigned);

	VirtualMachineHelper InitializeVm(VmProperties v, VirtualMachineHelper::VmType_e type);
	void AddToExecutedVmList(VmProperties p, uint32_t nodeId);
	void AddToSplitExecutedVmList(VmProperties actualVm, std::vector<VmProperties> splitVm, ComputationalNodeContainer nc);
	void AddToNotExecutedVmList(VmProperties p);
	void ConvertRatio();
	bool IsPortAssigned(uint32_t port);

	Storage GetStorageValue(Storage min, Storage max);
	ProcessingPower GetProcessingValue (ProcessingPower min, ProcessingPower max);
	ApplicationSize GetApplicationSize (ApplicationSize min, ApplicationSize max);
	uint32_t GetNumValue(uint32_t min, uint32_t max);
	Time GetTimeValue(Time min, Time max);
	double GetDoubleValue(double min, double max);
	DataRate GetDataRateValue(DataRate min, DataRate max);





private:

	std::vector<ExecutedVm_s> m_executedVm;
	std::vector<SplitExecutedVm_s> m_splitExecutedVm;
	std::vector<VmProperties> m_notExecutedVm;

	TracedCallback<ExecutedVm_s> m_executedVmTrace;
	TracedCallback<SplitExecutedVm_s> m_splitExecutedVmTrace;
	TracedCallback<VmProperties> m_notExecutedVmTrace;

};

} /* namespace ns3 */

#endif /* NUTSHELL_VM_SCHEDULER */
