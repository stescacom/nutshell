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
 * vm-data.cc
 *
 *  Created on: Jun 9, 2017
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#include "vm-data.h"

namespace ns3 {

VmData::VmData() {
	// TODO Auto-generated constructor stub

}

VmData::~VmData() {
	// TODO Auto-generated destructor stub
}

void
VmData::SetScheduler(VmScheduler & sch)
{
	m_schedulerObj = &sch;
}

void
VmData::ExecutedVmSink(ExecutedVm_s e)
{
	m_executedVm.push_back(e);
}

void
VmData::SplitExecutedVmSink(SplitExecutedVm_s se)
{
	m_splitExecutedVm.push_back(se);
}

void
VmData::NotExecutedVmSink(VmProperties vm)
{
	m_notExecutedVm.push_back(vm);
}

void
VmData::AttachTraceSink()
{
	NS_LOG_UNCOND("aTTACHING sinks");
	m_schedulerObj->TraceDisconnectWithoutContext("ExecutedVm", MakeCallback(&VmData::ExecutedVmSink, this));
	m_schedulerObj->TraceDisconnectWithoutContext("SplitExecutedVm", MakeCallback(&VmData::SplitExecutedVmSink, this));
	m_schedulerObj->TraceDisconnectWithoutContext("NotExecutedVm", MakeCallback(&VmData::NotExecutedVmSink, this));

//	Config::ConnectWithoutContext("/$ns3::VmScheduler/ExecutedVm", MakeCallback(&VmData::ExecutedVmSink, this));
//	Config::ConnectWithoutContext("/$ns3::VmScheduler/SplitExecutedVm", MakeCallback(&VmData::SplitExecutedVmSink, this));
//	Config::ConnectWithoutContext("/$ns3::VmScheduler/NotExecutedVm", MakeCallback(&VmData::NotExecutedVmSink, this));

}

void
VmData::WriteVmPropToFile(std::fstream & file, VmProperties p, std::string prepend)
{
	file << prepend << "Processing Power: \t" << p.processing << "\n";
	file << prepend << "Primary Storage: \t" << p.primary << "\n";
	file << prepend << "Secondary Storage: \t" << p.secondary << "\n";
	file << prepend << "Application Size: \t" << p.appSize << "\n";
	file << prepend << "Arrival Time: \t" << p.processing << "\n";
	file << prepend << "Require Data: \t" << p.requrieData << "\n";
	file << prepend << "Data Source: \t" << p.dataSource << "\n";
	file << prepend << "Data Amount: \t" << p.dataAmount << "\n";
	file << prepend << "HDD R/W Rate: \t" << p.hddRwRate << "\n";
	file << prepend << "RAM R/W Rate: \t" << p.memRwRate << "\n";
	file << prepend << "Processor RAM Access #: \t" << p.numOfProcAccesses << "\n";
	file << prepend << "HDD Access Time: \t" << p.hddAccessTime << "\n";
	file << prepend << "RAM Access Time: \t" << p.memAccessTime << "\n";
	file << prepend << "RAM % available for Data fetch: \t" << p.memPDF << "\n";
	file << prepend << "MTU: \t" << p.mtu << "\n";
	file << prepend << "Communication Protocol: \t" << p.cProtocolTid << "\n";
	file << prepend << "NIC Transmission Rate: \t" << p.transRate << "\n";
}

void
VmData::WriteExecutedVmToFile(std::fstream & file, ExecutedVm_s evm, std::string prepend)
{
	VmProperties p = evm.vm;
	file << "Arrival Time: \t" << p.arrivalTime << "\n";
	file << "Dispatched Time: \t" << evm.dispatched_time << "\n";
	file << "Node: \t" << evm.nid << "\n";
	WriteVmPropToFile(file, p, prepend);
}

void
VmData::ExportData()
{
	m_executedVm = m_schedulerObj->GetExecutedVmList();
	m_splitExecutedVm = m_schedulerObj->GetSplitExecutedVmList();
	m_notExecutedVm = m_schedulerObj->GetNotExecutedVmList();

	std::fstream file;
	std::string fileName = MakeFileName("executed-vm-data", ".txt");
	fileName = StringConcat(m_simulationPrefix, fileName);
	file.open(fileName.c_str(), std::fstream::out);
	if(m_executedVm.size() > 0)
	{
		for(uint32_t i = 0; i < m_executedVm.size(); i++)
		{
			ExecutedVm_s evm = m_executedVm[i];
			file << "-------------------------------------------------\n";
			WriteExecutedVmToFile(file, evm, "");
		}
	}
	else
	{
		file << "No VM was executed";
	}

	file.close();

	fileName = MakeFileName("split-executed-vm-data", ".txt");
	fileName = StringConcat(m_simulationPrefix, fileName);
	file.open(fileName.c_str(), std::fstream::out);

	if (m_splitExecutedVm.size() > 0)
	{
		for(uint32_t i = 0; i < m_splitExecutedVm.size(); i++)
		{
			SplitExecutedVm_s sevm = m_splitExecutedVm[i];
			VmProperties p = sevm.actualVm;
			file << "-------------------------------------------------\n";
			file << "Arrival Time: \t" << p.arrivalTime << "\n";
			file << "Dispatched Time (After all splits dispatched): \t" << sevm.dispatched_time << "\n";
			WriteVmPropToFile(file, p, "");

			std::vector<ExecutedVm_s> sevm_vec = sevm.splittedVm;

			for (uint32_t j = 0; sevm_vec.size(); j++)
			{
				file << "\t+++++++++++++++++++++++++++++++++++++++++++++\n";
				WriteExecutedVmToFile(file, sevm_vec[j], "\t");
			}
		}
	}

	else
	{
		file << "No VM was split";
	}

	file.close();

	fileName = MakeFileName("not-executed-vm-data", ".txt");
	fileName = StringConcat(m_simulationPrefix, fileName);
	file.open(fileName.c_str(), std::fstream::out);

	if(m_notExecutedVm.size() > 0)
	{
		for(uint32_t i = 0; i < m_notExecutedVm.size(); i++)
		{
			file << "-------------------------------------------------\n";
			WriteVmPropToFile(file, m_notExecutedVm[i], "");
		}
	}
	else
	{
		file << "No VM found in Not executed list";
	}


	file.close();
}

} /* namespace ns3 */
