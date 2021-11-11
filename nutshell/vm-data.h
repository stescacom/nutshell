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
 * vm-data.h
 *
 *  Created on: Jun 9, 2017
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#ifndef SCRATCH_NUTSHELL_VM_DATA_H_
#define SCRATCH_NUTSHELL_VM_DATA_H_

#include "vm-scheduler.h"
#include "nutshell-data-collector.h"

namespace ns3 {

class VmData: public NutshellDataCollector {
public:
	void SetScheduler(VmScheduler & sch);
	VmData();
	virtual ~VmData();

private:

	void ExecutedVmSink(ExecutedVm_s e);
	void SplitExecutedVmSink(SplitExecutedVm_s se);
	void NotExecutedVmSink(VmProperties vm);
	void WriteVmPropToFile(std::fstream & file, VmProperties p, std::string prepend);
	void WriteExecutedVmToFile(std::fstream & file, ExecutedVm_s evm, std::string prepend);
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


	std::vector<ExecutedVm_s> m_executedVm;
	std::vector<SplitExecutedVm_s> m_splitExecutedVm;
	std::vector<VmProperties> m_notExecutedVm;
	VmScheduler * m_schedulerObj;
};

} /* namespace ns3 */

#endif /* SCRATCH_NUTSHELL_VM_DATA_H_ */
