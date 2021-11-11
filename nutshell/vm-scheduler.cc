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
 * vm-scheduler.cc
 *
 *  Created on: Oct 23, 2015
 *      Author: ubaid
 */

#include <vector>
#include <stdlib.h>
#include <ctime>

#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/random-variable-stream.h"
#include "ns3/uinteger.h"
#include "ns3/boolean.h"
#include "ns3/double.h"

#include "processing-power-util.h"
#include "storage-util.h"
#include "application-size-util.h"
#include "datacenter-config.h"

#include "virtual-machine-helper.h"
#include "computational-node-container.h"

#include "virtual-machine.h"
#include "network-vm.h"
#include "computational-local-data-vm.h"
#include "producer-vm.h"
#include "consumer-producer-vm.h"
#include "consumer-vm.h"
#include "storage-server.h"
#include "vm-container.h"

#include "vm-scheduler.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("VmScheduler");

NS_OBJECT_ENSURE_REGISTERED (VmScheduler);

TypeId VmScheduler::GetTypeId ()
{
	static TypeId tid = TypeId("ns3::VmScheduler")
		.SetParent (Object::GetTypeId())
		.AddConstructor<VmScheduler> ()
		.AddTraceSource ("ExecutedVm",
						   "Trace for VM that are executed",
						   MakeTraceSourceAccessor (&VmScheduler::m_executedVmTrace))
	   .AddTraceSource ("SplitExecutedVm",
							   "Trace for VM that are split and executed",
							   MakeTraceSourceAccessor (&VmScheduler::m_splitExecutedVmTrace))
	   .AddTraceSource ("NotExecutedVm",
							   "Trace for VM that are not executed",
							   MakeTraceSourceAccessor (&VmScheduler::m_notExecutedVmTrace))
		;
	return tid;
}

VmScheduler::VmScheduler()
{
}

VmScheduler::VmScheduler(DatacenterConfig config)
{
	m_config = config;
}

void
VmScheduler::SetConfiguration(DatacenterConfig config)
{
	m_config = config;
}

void
VmScheduler::SetNodes(ComputationalNodeContainer c)
{
	m_dataCenterNodes = c;
}
void
VmScheduler::SetInterfaces(Ipv4InterfaceContainer iface)
{
	m_dcNodesInterfaces = iface;
}

void
VmScheduler::BeginScheduling()
{
	NS_LOG_INFO(this);
	if(m_vmList.size() == 0)
	{
		CreateVmList();
		SortVmList();
	}
	if(m_config.GetNumOfVmWithServerDataSrc() > 0 &&
			m_config.GetNumOfStorageServer() > 0)
	{
		CreateStorageServers();
	}

	if(m_config.IsVmRequiredData())
	{
		if(DatacenterConfig::RANDOM == m_config.GetVmDistributionType())
		{
			for(uint32_t i = 0; i < m_config.GetNumOfVmWithServerDataSrc(); i++)
			{
				uint32_t ri = GetNumValue(0, m_config.GetNumOfVmToCreate());

				if(m_vmList[ri].dataSource == STORAGE_SERVER)
				{
					ri = GetNumValue(0, m_config.GetNumOfVmToCreate());
				}
				m_vmList[ri].dataSource = STORAGE_SERVER;
			}
		}
	}
	while(m_vmList.size() > 0)
	{
		VmProperties vm = *m_vmList.begin();
		m_vmList.erase(m_vmList.begin());
		ScheduleVm(vm);
	}
}

void
VmScheduler::CreateVmList()
{
	for(uint32_t i = 0; i < m_config.GetNumOfVmToCreate(); i++)
	{
		VmProperties vm;

		/* ---------------------- General -----------------------*/

		vm.processing = GetProcessingValue(m_config.GetVmMinProcessing(), m_config.GetVmMaxProcessing());
		vm.primary = GetStorageValue(m_config.GetVmMinPrimaryStorage(), m_config.GetVmMaxPrimaryStorage());
		vm.secondary = GetStorageValue(m_config.GetVmMinSecondaryStorage(), m_config.GetVmMaxPrimaryStorage());
		vm.appSize = GetApplicationSize(m_config.GetVmMinAppSize(), m_config.GetVmMaxAppSize());

		/*----------------------- Data --------------------------*/
		vm.requrieData = m_config.IsVmRequiredData();
		if(vm.requrieData)
		{
			vm.dataAmount = GetStorageValue(m_config.GetVmDataAmountMin(), m_config.GetVmDataAmountMax());
			vm.hddRwRate = GetStorageValue(m_config.GetVmHddRwRateMin(), m_config.GetVmHddRwRateMax());
			vm.memRwRate = GetStorageValue(m_config.GetVmMemRwRateMin(), m_config.GetVmMemRwRateMax());
			vm.numOfProcAccesses = GetNumValue(m_config.GetVmNumOfProcAccessMin(), m_config.GetVmNumOfProcAccessMax());
			vm.memPDF = GetDoubleValue(m_config.GetVmMemPdfMin(), m_config.GetVmMemPdfMax());
			vm.hddAccessTime = GetTimeValue(m_config.GetVmHddMinAccessTime(), m_config.GetVmHddMaxAccessTime());
			vm.memAccessTime = GetTimeValue(m_config.GetVmMemMinAccessTime(), m_config.GetVmMemMaxAccessTime());
			vm.dataSource = LOCAL_DISK;
		}
		/* ---------------------- Arrival -----------------------*/
		vm.arrivalTime = GetTimeValue(m_config.GetVmArrivalTimeMin(), m_config.GetVmArrivalTimeMax());

		/*---------------------- Network -----------------------*/
		vm.transRate = GetDataRateValue(m_config.GetVmTransmissionRateMin(), m_config.GetVmTransmissionRateMax());
		vm.cProtocolTid = m_config.GetVmProtocolType();
		vm.mtu = m_config.GetVmMtu();

		if(m_ratio.size() == 0 &&
				m_config.IsVmSplitAllowed())
		{
			ConvertRatio();
		}

		m_vmList.push_back(vm);
	}

}

void
VmScheduler::CreateStorageServers()
{
	NS_LOG_INFO(this);
	uint32_t stIndex = m_dataCenterNodes.GetN() - m_config.GetNumOfStorageServer();
	for(uint32_t i =  stIndex; i < m_dataCenterNodes.GetN(); i++)
	{
		NS_LOG_INFO("Total number of storage servers " << m_config.GetNumOfStorageServer());

		Ptr<ComputationalNode> n = m_dataCenterNodes.Get(i);

		m_storageServers.Add(n);
		m_storageServersIfases.Add(m_dcNodesInterfaces.Get(i));

		Ptr<StorageServer> s = CreateObject<StorageServer>();
		s->SetLocal(Address(InetSocketAddress(Ipv4Address::GetAny(), 3000)));
		uint16_t mtu = (uint16_t ) m_config.GetVmMtu();
		s->SetMtu(mtu);
		s->SetProtocol(m_config.GetVmProtocolType());
		n->AddApplication(s);

		s->SetStartTime(Simulator::Now());
	}
}

void
VmScheduler::SortVmList()
{
	NS_LOG_INFO(this);

	for(uint32_t i = 0; i < m_vmList.size(); i++)
	{
		VmProperties tmp;
		for(uint32_t j = i + 1; j < m_vmList.size(); j++)
		{
			if(m_vmList[j].arrivalTime < m_vmList[i].arrivalTime)
			{
				tmp = m_vmList[i];
				m_vmList[i] = m_vmList[j];
				m_vmList[j] = tmp;
			}
		}
	}
}

VirtualMachineHelper
VmScheduler::InitializeVm(VmProperties v, VirtualMachineHelper::VmType_e type)
{
	VirtualMachineHelper vmh(type);
	/*----------------------General ----------------------*/
	vmh.SetAttribute("ProcessingPower", ProcessingPowerValue(v.processing));
	vmh.SetAttribute("PrimaryStorage", StorageValue(v.primary));
	vmh.SetAttribute("SecondaryStorage", StorageValue(v.secondary));
	vmh.SetAttribute("ApplicationSize", ApplicationSizeValue(v.appSize));

	/*-------------------- Data -----------------------*/
	if(v.requrieData)
	{

		vmh.SetAttribute("HddRwRate", StorageValue(v.hddRwRate));
		vmh.SetAttribute("MemRwRate", StorageValue(v.memRwRate));
		vmh.SetAttribute("NumOfAccesses", UintegerValue(v.numOfProcAccesses));
		vmh.SetAttribute("RamPercentageForData", DoubleValue(v.memPDF));
		vmh.SetAttribute("MemoryAccessTime", TimeValue(v.memAccessTime));
		vmh.SetAttribute("HddAccessTime", TimeValue(v.hddAccessTime));

		if(type == VirtualMachineHelper::COMPUTATIONAL_LOCAL_DATA)
		{
			vmh.SetAttribute("RequireData", BooleanValue(v.requrieData));
			vmh.SetAttribute("DataAmount", StorageValue(v.dataAmount));
		}
		else if(type == VirtualMachineHelper::CONSUMER_PRODUCER ||
				type == VirtualMachineHelper::CONSUMER)
		{
			vmh.SetAttribute("UseLocalDataProcessing", BooleanValue(v.requrieData));
			vmh.SetAttribute("DataSize", StorageValue(v.dataAmount));
		}
	}
	else
	{
		vmh.SetAttribute("RequireData", BooleanValue(v.requrieData));
	}

	/* ------------------ Network --------------*/
	if(type == VirtualMachineHelper::PRODUCER ||
			type == VirtualMachineHelper::CONSUMER_PRODUCER ||
				type == VirtualMachineHelper::CONSUMER)
	{
		vmh.SetAttribute("SocketType", TypeIdValue(TypeId::LookupByName(v.cProtocolTid)));
		vmh.SetAttribute("Mtu", UintegerValue(v.mtu));
		vmh.SetAttribute("VmTransmissionRate", DataRateValue(v.transRate));
	}

	return vmh;
}


void
VmScheduler::ScheduleVm(VmProperties & vm)
{
	Simulator::Schedule(vm.arrivalTime,
			&VmScheduler::DispatchVmOnNode, this, vm);
}

void
VmScheduler::DispatchVmOnNode(VmProperties & vm)
{
	NS_LOG_INFO("Dispatching... ");

	if(m_portAssigned.size() == 0)
	{
		m_portAssigned.push_back(3000);
	}

	if(vm.requrieData)
	{
		if(vm.dataAmount > vm.secondary)
		{
			AddToNotExecutedVmList(vm);
			return;
		}
	}

	Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable>();
	uint32_t listeningPort = 0;
	bool portAssigned = true;
	while(portAssigned)
	{
		listeningPort = uv->GetValue(2048, 65535);
		NS_LOG_INFO("Looking up" << listeningPort);
		portAssigned = IsPortAssigned(listeningPort);
	}

	Ptr<ComputationalNode> n;
	ComputationalNodeContainer n4vmSplit;
	bool foundFirstFit = false;

	for(ComputationalNodeContainer::Iterator i = m_dataCenterNodes.Begin();
			i != (m_dataCenterNodes.End() - m_config.GetNumOfStorageServer()); i++)
	{
		n = *i;

		if(n->CheckAvailability(vm.processing, vm.primary, vm.secondary, vm.transRate))
		{
			VirtualMachineHelper dispVm(VirtualMachineHelper::COMPUTATIONAL_LOCAL_DATA);
			if(vm.requrieData)
			{
				if(vm.dataSource == STORAGE_SERVER)
				{
					uint32_t addIndex = 0;
					if(DatacenterConfig::RANDOM == m_config.GetVmDistributionType())
					{
						Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable>();
						addIndex = uv->GetValue(0, m_config.GetNumOfStorageServer());

					}
					NS_LOG_INFO("The add index: " << addIndex);
					dispVm = InitializeVm(vm, VirtualMachineHelper::CONSUMER);

					dispVm.SetAttribute("ConsumerType", EnumValue(ConsumerVm::CONSUMER_CLIENT));

					dispVm.SetAttribute("RemoteAddress", AddressValue(Address(m_storageServersIfases.GetAddress(addIndex))));
					dispVm.SetAttribute("RemotePort", UintegerValue(3000));

					dispVm.SetAttribute("ListeningAddress", AddressValue(Address(Ipv4Address::GetAny())));
					dispVm.SetAttribute("ListeningPort", UintegerValue(listeningPort));
				}
				else
				{
					dispVm = InitializeVm(vm, VirtualMachineHelper::COMPUTATIONAL_LOCAL_DATA);
				}
			}
			else
			{
				dispVm = InitializeVm(vm, VirtualMachineHelper::COMPUTATIONAL_LOCAL_DATA);
			}
			VmContainer vmInstalled = dispVm.Install(n);
			vmInstalled.Start(Simulator::Now());

			if(vmInstalled.Get(0)->IsResourcesReserved())
			{
				foundFirstFit = true;
				AddToExecutedVmList(vm, n->GetId());
				break;
			}
		}
	}


	if(!foundFirstFit &&
			m_config.IsVmSplitAllowed())
	{
		if(m_ratio.size() > 0 &&
				m_ratio.size() <= (m_dataCenterNodes.GetN() - m_config.GetNumOfStorageServer()))
		{
			foundFirstFit = SplitAndStart(vm);
		}

	}

	if(!foundFirstFit)
	{
		AddToNotExecutedVmList(vm);
	}
}

bool
VmScheduler::SplitAndStart(VmProperties & vm)
{
	NS_LOG_INFO("Its here");
	std::vector<bool> assigned;
	std::vector<VmProperties> splittedVmVec;

	ComputationalNodeContainer nodesForSplit;
	Ipv4InterfaceContainer nodesForSplitIface;

	SplitVm(vm, splittedVmVec);
	FindNodeIndicies(splittedVmVec, nodesForSplit, nodesForSplitIface);

	if(nodesForSplit.GetN() == m_ratio.size())
	{
		StartVm(splittedVmVec, nodesForSplit, nodesForSplitIface, assigned);
	}
	else
	{
		return false;
	}

	if(assigned.size() == m_ratio.size())
	{
		AddToSplitExecutedVmList(vm, splittedVmVec, nodesForSplit);
		return true;
	}
	else
	{
		return false;
	}
}

void
VmScheduler::SplitVm(VmProperties & vm, std::vector<VmProperties> & spvec)
{

	for(std::vector<uint32_t>::iterator i = m_ratio.begin(); i != m_ratio.end(); i++)
	{
		VmProperties splittedVmProperties = vm;

		double num = (double) *i;
		double denom = (double) m_ratioSum;

		double rs = (num / denom);

		splittedVmProperties.primary = Storage(rs * vm.primary.GetStorage());
		splittedVmProperties.secondary = Storage(rs * vm.secondary.GetStorage());

		uint64_t splitProcessing =  rs * vm.processing.GetProcessingPower();
		uint64_t splitAppSize = rs * vm.appSize.GetApplicationSize();

		if(vm.processing.IsFlops())
		{
			splittedVmProperties.processing = ProcessingPower(splitProcessing, ProcessingPower::POWER_FLOPS);
		}
		else if(vm.processing.IsMips())
		{
			splittedVmProperties.processing = ProcessingPower(splitProcessing, ProcessingPower::POWER_MIPS);
		}

		if(vm.appSize.IsFlop())
		{
			splittedVmProperties.appSize = ApplicationSize(splitAppSize, "flop");
		}
		else if(vm.appSize.IsInstruction())
		{
			splittedVmProperties.appSize = ApplicationSize(splitAppSize, "ic");
		}
		spvec.push_back(splittedVmProperties);
	}
}

void
VmScheduler::FindNodeIndicies(std::vector<VmProperties> & vmSplitted,
		ComputationalNodeContainer & selectedNodes, Ipv4InterfaceContainer & selectedNodeIface)
{
	uint32_t nodeLimit = m_dataCenterNodes.GetN() - m_config.GetNumOfStorageServer();
	for(uint32_t i = 0; i < vmSplitted.size(); i++)
	{
		VmProperties svp = vmSplitted[i];
		Ptr<ComputationalNode> n;

		for(uint32_t j = 0; j < nodeLimit; j++)
		{
			n = m_dataCenterNodes.Get(j);
			bool alreadySelected = false;

			/*
			 * Check if there is a node selected
			 * if there is then loop through and find
			 * if the current node counter is selected
			 * for other part of VM.
			 */
			if(selectedNodes.GetN() > 0 )
			{
				for(uint32_t f = 0; f < selectedNodes.GetN(); f++)
				{
					if(n == selectedNodes.Get(f))
					{
						alreadySelected = true;
						break;
					}
				}
			}
			/*
			 * If the index is already selected
			 * increment the counter and continue
			 */
			if(alreadySelected)
			{
				continue;
			}

			/*
			 * Check the availability of resources
			 */

			if(n->CheckAvailability(svp.processing,	svp.primary, svp.secondary, svp.transRate))
			{
				if(selectedNodes.GetN() != vmSplitted.size())
				{
					selectedNodes.Add(n);
					selectedNodeIface.Add(m_dcNodesInterfaces.Get(j));
					break;
				}
				else
				{
					break;
				}
			}
		}
	}
}

void
VmScheduler::StartVm(std::vector<VmProperties> & splittedVmVec,
		ComputationalNodeContainer & selectedNodes, Ipv4InterfaceContainer & selectedNodeIface,
			std::vector<bool> & assigned)
{
	NS_LOG_FUNCTION(this);
	/*
	 * Calculate unique ports for VM's listening and transmission
	 */
	Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable>();
	std::vector<uint32_t> ports;
	for(uint32_t i = 0; i < splittedVmVec.size(); i++)
	{
		uint32_t p = 0;
		bool portAssigned = true;
		while(portAssigned)
		{
			p = uv->GetValue(2048, 65535);
			portAssigned = IsPortAssigned(p);
		}
		m_portAssigned.push_back(p);
		ports.push_back(p);
	}

	VmContainer splitVmContainer;

	for(uint32_t i = 0; i < splittedVmVec.size(); i++)
	{
		VmProperties svmp = splittedVmVec[i];
		VirtualMachineHelper vmh (VirtualMachineHelper::COMPUTATIONAL_LOCAL_DATA);
		if(svmp.requrieData)
		{
			// requires data
			NS_LOG_INFO("requires data");
			if(svmp.dataSource == STORAGE_SERVER)
			{
				NS_LOG_INFO("data from storage");
				uint32_t addIndex = 0;
				if(DatacenterConfig::RANDOM == m_config.GetVmDistributionType())
				{
					addIndex = uv->GetValue(0, m_config.GetNumOfStorageServer());
				}
				// data is required from storage server
				if(i >= 0 && i < (splittedVmVec.size() - 1))
				{
					NS_LOG_INFO("VM type ConsumerProducer");
					vmh = InitializeVm(svmp, VirtualMachineHelper::CONSUMER_PRODUCER);
					// index for consumer producer
					if(i == 0)
					{
						NS_LOG_INFO("Consumer to request data from server");
						// consumer producer to get data from server
						vmh.SetAttribute("ConsumerType", EnumValue(ConsumerProducerVm::CONSUMER_CLIENT));

						vmh.SetAttribute("RemoteAddress", AddressValue(Address(m_storageServersIfases.GetAddress(addIndex))));
						vmh.SetAttribute("RemotePort", UintegerValue(3000));

						vmh.SetAttribute("ListeningAddress", AddressValue(Address(Ipv4Address::GetAny())));
						vmh.SetAttribute("ListeningPort", UintegerValue(ports[i]));

						vmh.SetAttribute("ProducerForRemoteAddress", AddressValue(Address(selectedNodeIface.GetAddress(i+1))));
						vmh.SetAttribute("ProducerForRemotePort", UintegerValue(ports[i+1]));

					}
					else
					{
						NS_LOG_INFO("Consumer to request data from producer");
						// consumer producer to wait for data from consumer producer
						vmh.SetAttribute("ConsumerType", EnumValue(ConsumerProducerVm::CONSUMER_CONSUMER));

						// set producer address and its listener port for producer socket
						vmh.SetAttribute("RemoteAddress", AddressValue(Address(selectedNodeIface.GetAddress(i-1))));
						vmh.SetAttribute("RemotePort", UintegerValue(ports[i-1]));

						vmh.SetAttribute("ListeningAddress", AddressValue(Address(Ipv4Address::GetAny())));
						vmh.SetAttribute("ListeningPort", UintegerValue(ports[i]));

						vmh.SetAttribute("ProducerForRemoteAddress", AddressValue(Address(selectedNodeIface.GetAddress(i+1))));
						vmh.SetAttribute("ProducerForRemotePort", UintegerValue(ports[i+1]));

					}
				}
				else
				{
					// consumer only
					vmh = InitializeVm(svmp, VirtualMachineHelper::CONSUMER);
					// consumer producer to wait for data from consumer producer
					vmh.SetAttribute("ConsumerType", EnumValue(ConsumerVm::CONSUMER_CONSUMER));

					// set producer address and its listener port for producer socket
					vmh.SetAttribute("RemoteAddress", AddressValue(Address(selectedNodeIface.GetAddress(i-1))));
					vmh.SetAttribute("RemotePort", UintegerValue(ports[i-1]));

					vmh.SetAttribute("ListeningAddress", AddressValue(Address(Ipv4Address::GetAny())));
					vmh.SetAttribute("ListeningPort", UintegerValue(ports[i]));

				}

			}
			else
			{

				if(i == 0)
				{
					// producer
					NS_LOG_INFO("Producer");
					vmh = InitializeVm(svmp, VirtualMachineHelper::PRODUCER);

					vmh.SetAttribute("RemoteAddress", AddressValue(Address(selectedNodeIface.GetAddress(i+1))));
					vmh.SetAttribute("RemotePort", UintegerValue(ports[i+1]));

					vmh.SetAttribute("ListeningAddress", AddressValue(Ipv4Address::GetAny()));
					vmh.SetAttribute("ListeningPort", UintegerValue(ports[i]));

				}
				else if(i > 0 && i < (splittedVmVec.size() - 1))
				{
					NS_LOG_INFO("COnsumer producer ");
					//consumer producer
					vmh = InitializeVm(svmp, VirtualMachineHelper::CONSUMER_PRODUCER);

					vmh.SetAttribute("ConsumerType", EnumValue(ConsumerProducerVm::CONSUMER_CONSUMER));

					// set producer address and its listener port for producer socket
					vmh.SetAttribute("RemoteAddress", AddressValue(selectedNodeIface.GetAddress(i-1)));
					vmh.SetAttribute("RemotePort", UintegerValue(ports[i-1]));

					vmh.SetAttribute("ListeningAddress", AddressValue(Ipv4Address::GetAny()));
					vmh.SetAttribute("ListeningPort", UintegerValue(ports[i]));

					vmh.SetAttribute("ProducerForRemoteAddress", AddressValue(Address(selectedNodeIface.GetAddress(i+1))));
					vmh.SetAttribute("ProducerForRemotePort", UintegerValue(ports[i+1]));
				}
				else
				{
					// consumer
					NS_LOG_INFO("Consumer only");
					vmh = InitializeVm(svmp, VirtualMachineHelper::CONSUMER);
					// consumer producer to wait for data from consumer producer
					vmh.SetAttribute("ConsumerType", EnumValue(ConsumerVm::CONSUMER_CONSUMER));

					// set producer address and its listener port for producer socket
					vmh.SetAttribute("RemoteAddress", AddressValue(Address(selectedNodeIface.GetAddress(i-1))));
					vmh.SetAttribute("RemotePort", UintegerValue(ports[i-1]));

					vmh.SetAttribute("ListeningAddress", AddressValue(Address(Ipv4Address::GetAny())));
					vmh.SetAttribute("ListeningPort", UintegerValue(ports[i]));
				}
			}

			// now assign the vm to node

			Ptr<ComputationalNode> n = selectedNodes.Get(i);
			VmContainer vmc = vmh.Install(n);
			if(vmc.Get(0)->IsResourcesReserved())
			{
				splitVmContainer.Add(vmc);
				assigned.push_back(true);
			}
		}
		else
		{
			vmh = InitializeVm(svmp, VirtualMachineHelper::COMPUTATIONAL_LOCAL_DATA);
			Ptr<ComputationalNode> n = selectedNodes.Get(i);
			VmContainer vmc = vmh.Install(n);
			if(vmc.Get(0)->IsResourcesReserved())
			{
				splitVmContainer.Add(vmc);
				assigned.push_back(true);
			}
		}
	}
	splitVmContainer.Start(Simulator::Now());
}

void
VmScheduler::AddToExecutedVmList(VmProperties p, uint32_t nodeId)
{
	ExecutedVm_s evm;
	evm.vm = p;
	evm.nid = nodeId;
	evm.dispatched_time = Simulator::Now().GetSeconds();
	m_executedVm.push_back(evm);
	m_executedVmTrace(evm);
}

void
VmScheduler::AddToSplitExecutedVmList(VmProperties actualVm, std::vector<VmProperties> splitVm, ComputationalNodeContainer nc)
{
	NS_LOG_UNCOND("Added to SEVM");
	SplitExecutedVm_s sevm;
	sevm.actualVm = actualVm;
	for(uint32_t i = 0; i < splitVm.size(); i++)
	{
		ExecutedVm_s evm;
		evm.vm = splitVm[i];
		evm.nid = nc.Get(i)->GetId();
		evm.dispatched_time = Simulator::Now().GetSeconds();
		sevm.splittedVm.push_back(evm);
	}
	sevm.dispatched_time = Simulator::Now().GetSeconds();
	m_splitExecutedVm.push_back(sevm);
	m_splitExecutedVmTrace(sevm);

}

void
VmScheduler::AddToNotExecutedVmList(VmProperties p)
{
	NS_LOG_UNCOND("Added to NEVM");
	m_notExecutedVm.push_back(p);
	m_notExecutedVmTrace(p);
}

void
VmScheduler::ConvertRatio()
{
	std::string r = m_config.GetVmSplitRatio();
	m_ratioSum = 0;
//	NS_LOG_UNCOND("Remaining String " << r);
	std::string::size_type i;
	while(r.length() > 0)
	{
		std::istringstream iss;
		uint32_t tval;
		i = r.find_first_of(":");
		if(i != std::string::npos)
		{
			iss.str(r.substr(0, i));
			iss >> tval;
			m_ratio.push_back(tval);
			m_ratioSum = m_ratioSum + tval;
			r = r.substr(++i, std::string::npos);
		}
		else
		{
			iss.str(r);
			iss >> tval;
			m_ratioSum = m_ratioSum + tval;
			m_ratio.push_back(tval);
			break;
		}
	}
}

bool
VmScheduler::IsPortAssigned(uint32_t port)
{
	bool found = false;
	for(std::vector<uint32_t>::iterator i = m_portAssigned.begin();
			i != m_portAssigned.end(); i++)
	{
		NS_LOG_INFO("The ports" << *i);
		if(*i == port)
		{
			found = true;
			break;
		}
	}
	if(found)
	{
		return true;
	}
	else
	{
		return false;
	}
}

Storage
VmScheduler::GetStorageValue(Storage min, Storage max)
{
	if(min == max)
	{
		return min;
	}
	else
	{
		Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable>();
		uint64_t newVal = uv->GetValue(min.GetStorage(), max.GetStorage());
		return Storage(newVal);
	}
}
ProcessingPower
VmScheduler::GetProcessingValue (ProcessingPower min, ProcessingPower max)
{
	if(min == max)
	{
		return min;
	}
	else
	{
		Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable>();
		uint64_t newVal = uv->GetValue(min.GetProcessingPower(), max.GetProcessingPower());
		int metric;
		if(min.IsFlops() && max.IsFlops())
		{
			metric = ProcessingPower::POWER_FLOPS;
		}
		else if (min.IsMips() && max.IsMips())
		{
			metric = ProcessingPower::POWER_MIPS;
		}
		else
		{
			NS_FATAL_ERROR("Processing Powers do not match " << this);
		}

		return ProcessingPower(newVal, metric);
	}
}

ApplicationSize
VmScheduler::GetApplicationSize(ApplicationSize min, ApplicationSize max)
{
	if(min == max)
	{
		return min;
	}
	else
	{
		Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable>();
		uint64_t newVal = uv->GetValue(min.GetApplicationSize(), max.GetApplicationSize());
		std::string metric;
		if(min.IsFlop() && max.IsFlop())
		{
			metric = "flop";
		}
		else if( min.IsInstruction() && min.IsInstruction() )
		{
			metric = "ic";
		}
		else
		{
			NS_FATAL_ERROR("The application instruction metric do not match " << this);
		}
		return ApplicationSize(newVal, metric);
	}
}

uint32_t
VmScheduler::GetNumValue(uint32_t min, uint32_t max)
{
	if(min == max)
	{
		return min;
	}
	else
	{
		Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable>();
		return uv->GetValue(min, max);
	}
}
Time
VmScheduler::GetTimeValue(Time min, Time max)
{
	if(min == max)
	{
		return min;
	}
	else
	{
		Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable>();
		uint64_t mins = min.GetSeconds();
		uint64_t maxs = max.GetSeconds();
		double val = uv->GetValue(mins, maxs);
		return Time::From(val, Time::S);
	}
}
double
VmScheduler::GetDoubleValue(double min, double max)
{
	if(min == max)
	{
		return min;
	}
	else
	{
		Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable>();
		return uv->GetValue(min, max);
	}
}

DataRate
VmScheduler::GetDataRateValue(DataRate min, DataRate max)
{
	if(min == max)
	{
		return min;
	}
	else
	{
		Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable>();
		uint64_t rate = uv->GetValue(min.GetBitRate(), max.GetBitRate());
		return DataRate(rate);
	}
}


uint32_t
VmScheduler::GetExecutedVmNum()
{
	return m_executedVm.size();
}
uint32_t
VmScheduler::GetNotExecutedVmNum()
{
	return m_notExecutedVm.size();
}

std::vector<ExecutedVm_s>
VmScheduler::GetExecutedVmList()
{
	return m_executedVm;
}
std::vector<SplitExecutedVm_s>
VmScheduler::GetSplitExecutedVmList()
{
	return m_splitExecutedVm;
}
std::vector<VmProperties>
VmScheduler::GetNotExecutedVmList()
{
	return m_notExecutedVm;
}

VmScheduler::~VmScheduler()
{
}

} /* namespace ns3 */
