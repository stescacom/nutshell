/*
 * computational-local-data-vm.cc
 *
 *  Created on: Aug 19, 2015
 *      Author: ubaid
 */

#include "computational-local-data-vm.h"

#include "ns3/log.h"
#include "ns3/address.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/packet-socket-address.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/random-variable-stream.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/string.h"
#include "ns3/pointer.h"
#include "ns3/boolean.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("ComputationalLocalDataVm");

NS_OBJECT_ENSURE_REGISTERED(ComputationalLocalDataVm);

TypeId
ComputationalLocalDataVm::GetTypeId()
{
	static TypeId tid = TypeId("ns3::ComputationalLocalDataVm")
					.SetParent<VirtualMachine>()
					.AddConstructor<ComputationalLocalDataVm>()
					.AddAttribute("RequireData", "Does the Virtual Machine require data for its operations",
									BooleanValue(false),
									MakeBooleanAccessor(&ComputationalLocalDataVm::m_requireData),
									MakeBooleanChecker())
					.AddAttribute("DataAmount",
									"Required amount of data for VM operation",
									StorageValue(Storage(0)),
									MakeStorageAccessor(&ComputationalLocalDataVm::m_dataAmount),
									MakeStorageChecker())
					.AddAttribute("HddRwRate",
									"Rate at which data is read/written from/to secondary storage",
									StorageValue(Storage("300MB")),
									MakeStorageAccessor(&ComputationalLocalDataVm::m_hddRwRate),
									MakeStorageChecker())
					.AddAttribute("MemRwRate",
									"Rate at which data is read/written from/to primary storage",
									StorageValue(Storage("9600MB")),
									MakeStorageAccessor(&ComputationalLocalDataVm::m_memRwRate),
									MakeStorageChecker())
					.AddAttribute("NumOfAccesses",
									"Number of access to RAM by processor for data",
									UintegerValue(100),
									MakeUintegerAccessor(&ComputationalLocalDataVm::m_numOfAcc),
									MakeUintegerChecker<uint32_t>(1))
					.AddAttribute("RamPercentageForData",
									"Percentage of RAM available for Data storage that was read from secondary storage",
									DoubleValue(80.0),
									MakeDoubleAccessor(&ComputationalLocalDataVm::m_ramPDF),
									MakeDoubleChecker<double>(10.0))
					.AddAttribute("MemoryAccessTime",
									"Access time of primary storage to access the data required",
									TimeValue(NanoSeconds(100.0)),
									MakeTimeAccessor(&ComputationalLocalDataVm::m_memAccTime),
									MakeTimeChecker(NanoSeconds(50.0)))
					.AddAttribute("HddAccessTime",
									"Access time of secondary storage to access the required data",
									TimeValue(MilliSeconds(12)),
									MakeTimeAccessor(&ComputationalLocalDataVm::m_hddAccTime),
									MakeTimeChecker())
									;
	return tid;
}

ComputationalLocalDataVm::ComputationalLocalDataVm()
	: m_dataAmount(Storage(0))
{
	NS_LOG_FUNCTION(this);
	m_requireData = false;
}

ComputationalLocalDataVm::~ComputationalLocalDataVm()
{
	NS_LOG_FUNCTION(this);
}

/*
 * -------------------- setters and getters -----------
 */
void
ComputationalLocalDataVm::SetRequiredData(const bool rData)
{
	m_requireData = rData;
}
void
ComputationalLocalDataVm::SetDataAmount(const Storage& amt)
{
	if(m_dataAmount <= this->GetSecondaryStorage())
	{
		m_dataAmount = amt;
	}
	else
	{
		NS_FATAL_ERROR("Required data exceeds Virtual Machine's storage capacity");
	}

}
void
ComputationalLocalDataVm::SetHddRwRate(const Storage rate)
{
	m_hddRwRate = rate;
}

void
ComputationalLocalDataVm::SetMemRwRate(const Storage rate)
{
	m_memRwRate = rate;
}
void
ComputationalLocalDataVm::SetNumOfAccesses(const uint32_t v)
{
	m_numOfAcc = v;
}
void
ComputationalLocalDataVm::SetRamPDF(const double percentage)
{
	m_ramPDF = percentage;
}
void
ComputationalLocalDataVm::SetMemAccTime(const Time t)
{
	m_memAccTime = t;
}
void
ComputationalLocalDataVm::SetHddAccTime(const Time t)
{
	m_hddAccTime = t;
}

bool
ComputationalLocalDataVm::GetRequiredData() const
{
	return m_requireData;
}
Storage
ComputationalLocalDataVm::GetDataAmount() const
{
	return m_dataAmount;
}
Storage
ComputationalLocalDataVm::GetHddRwRate() const
{
	return m_hddRwRate;
}

Storage
ComputationalLocalDataVm::GetMemRwRate() const
{
	return m_memRwRate;
}
uint32_t
ComputationalLocalDataVm::GetNumofAccesses() const
{
	return m_numOfAcc;
}
double
ComputationalLocalDataVm::GetRamPDF() const
{
	return m_ramPDF;
}
Time
ComputationalLocalDataVm::GetMemAccTime() const
{
	return m_memAccTime;
}
Time
ComputationalLocalDataVm::GetHddAccTime() const
{
	return m_hddAccTime;
}

/*
 * -------end of setters and getters-------------
 */

/*
 * ----------protected methods---------------
 */

void
ComputationalLocalDataVm::DoDispose()
{
	NS_LOG_FUNCTION(this);
	VirtualMachine::DoDispose();
}

void
ComputationalLocalDataVm::ScheduleStop(double time)
{
	NS_LOG_FUNCTION(this);
	Simulator::Schedule(Seconds(time), &ComputationalLocalDataVm::StopApplication, this);
}


/*
 * ------end of protected methods-------------
 */


/*
 * ----------private methods---------------
 */

void
ComputationalLocalDataVm::StartApplication()
{
	NS_LOG_FUNCTION(this);
	if(ReserveResources())
	{
		NS_LOG_INFO("ComputationalLocalDataVm started at "<< Simulator::Now().GetSeconds());
		if(m_requireData)
		{
			double dataFetch = CalculateDataFetchTime();
			double processingTime = CalculateProcessingTime();
			ScheduleStop((dataFetch+processingTime));
		}
		else
		{
			double processingTime = CalculateProcessingTime();
			ScheduleStop(processingTime);
		}
	}
	else
	{
		StopApplication();
	}

}

void
ComputationalLocalDataVm::StopApplication()
{
	NS_LOG_FUNCTION(this);
	if(m_reservedResources)
	{
		ReleaseResources();
		NS_LOG_INFO("ComputationalLocalDataVm stopped at "<< Simulator::Now().GetSeconds());
	}

}

double
ComputationalLocalDataVm::CalculateDataFetchTime()
{
	double totalTime = 0;

	// available RAM size for data
	uint64_t RamAvail = (GetPrimaryStorage().GetStorage() * m_ramPDF) /100 ;

	// number of access from RAM to HDD
	uint32_t numAccMem2Hdd = m_dataAmount.GetStorage()/RamAvail;

	// data size per access from RAM to Processor
	uint64_t ram2ProcessorDataSize = m_dataAmount.GetStorage() / m_numOfAcc;

	/*
	 * Formulas
	 * total time of Data fetch from HDD to RAM = (no of HDD access * HDD access time) + (available RAM size / HDD RW rate)
	 *
	 * total time of data fetch from RAM to Processor = (no of RAM access * RAM access time) + (RAM to Processor read data size / RAM RW rate)
	 */

	double tHdd2RAM, tRAM2Proc;

	tHdd2RAM = numAccMem2Hdd * (m_hddAccTime.GetSeconds() + (RamAvail / m_hddRwRate.GetStorage()));
	tRAM2Proc = m_numOfAcc * (m_memAccTime.GetSeconds() + (ram2ProcessorDataSize / m_memRwRate.GetStorage()));

	totalTime = tHdd2RAM + tRAM2Proc;

	return totalTime;
}

/*
 * ------end of private methods-------------
 */

} /* namespace ns3 */
