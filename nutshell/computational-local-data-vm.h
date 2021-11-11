/*
 * computational-local-data-vm.h
 *
 *  Created on: Aug 19, 2015
 *      Author: ubaid
 */

#ifndef COMPUTATIONAL_LOCAL_DATA_VM_H
#define COMPUTATIONAL_LOCAL_DATA_VM_H

#include "storage-util.h"
#include "virtual-machine.h"
#include "ns3/application.h"
#include "ns3/address.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/data-rate.h"
#include "ns3/traced-callback.h"
#include "ns3/nstime.h"


namespace ns3 {
/**
 * \brief Creates a Virtual Machine with an application that perform its execution with data provided locally.
 *
 * The class represents VM that contain an application of User that is meant to
 * perform some computation and require data that is available locally on the VM's
 * Secondary Storage. The VM closes itself after completing its execution.
 *
 */
class ComputationalLocalDataVm: public VirtualMachine {
public:
	/**
	* \brief Get the type ID.
	* \return the object TypeId
	*/
	static TypeId GetTypeId();
	/**
	 * \brief Class constructor
	 */
	ComputationalLocalDataVm();

	virtual ~ComputationalLocalDataVm();
	/**
	 * \brief Sets a flag if the VM application require data
	 * \param rData the flag value, true if require data, false otherwise
	 */
	void SetRequiredData(const bool rData);
	/**
	 * \brief Sets the amount of data required by the application
	 * \param amt the data size
	 */
	void SetDataAmount(const Storage& amt);
	/**
	 * \brief Sets the Hard Disk Drive (HDD) Read/Write Rate
	 * \param rate the data size to read/write per second
	 */
	void SetHddRwRate(const Storage rate);
	/**
	 * \brief Sets Memory (RAM) read/write rate
	 * \param rate the data size to read/write per second
	 */
	void SetMemRwRate(const Storage rate);
	/**
	 * \brief Sets the Number of access
	 *
	 * The number of access attempts made by processor to fetch data from
	 * RAM, which is required by the application.
	 *
	 * \param v the number of access value.
	 *
	 */
	void SetNumOfAccesses(const uint32_t v);
	/**
	 * \brief Sets the amount of RAM available for data fetching
	 *
	 * The value required is in percentage, e.g if 30% of RAM is available
	 * for data fetching, then a 30 value must be passed.
	 * \param percentage the percentage of available RAM
	 */
	void SetRamPDF(const double percentage);
	/**
	 * \brief Sets the RAM access time
	 *
	 * The access time refers to the time taken to access information
	 * on RAM. This will simulate the delay that can occur when reading
	 * data.
	 * \param t The amount of time.
	 */
	void SetMemAccTime(const Time t);
	/**
	 * \brief Sets the HDD access time
	 * The access time refers to the time taken to access information
	 * on HDD. This will simulate the delay that can occur when reading
	 * data.
	 * \param t The amount of time.
	 */
	void SetHddAccTime(const Time t);
	/**
	 * \brief Get the data requirement flag value
	 * \return the value of data requirement flag
	 */
	bool GetRequiredData() const;
	/**
	 * \brief Get the amount of data
	 * \return the amount of data required by the application
	 */
	Storage GetDataAmount() const;
	/**
	 * \brief Get the HDD Read/Write Rate.
	 * \return the amount of data that a HDD read/write per second.
	 */
	Storage GetHddRwRate() const;
	/**
	 * \brief Get Memory Read/Write Rate
	 * \return the amount of data that a RAM read/write per second.
	 */
	Storage GetMemRwRate() const;
	/**
	 * \brief Gets the number of Processor to RAM accesses
	 * \return the number of accesses
	 */
	uint32_t GetNumofAccesses() const;
	/**
	 * \brief Get the percentage of RAM available for Data Fetching
	 * \return the percentage value of available RAM.
	 */
	double GetRamPDF() const;
	/**
	 * \brief Get the memory access time
	 * \return the amount of time an information is accessible on RAM
	 */
	Time GetMemAccTime() const;
	/**
	 * \brief Get the HDD access time
	 * \return the amount of time an information is accessible on HDD
	 */
	Time GetHddAccTime() const;

protected:
	virtual void DoDispose();
	virtual void ScheduleStop(double time);
private:
	bool 				m_requireData; //!< Flag to set if the VM require data
	Storage				m_dataAmount; //!< Amount of data required by VM application
	Storage				m_hddRwRate; //!< HDD read/write rate
	Storage				m_memRwRate; //!< RAM read/write rate

	uint32_t 			m_numOfAcc;	//!< Processor request for data to RAM
	double				m_ramPDF;	//!< RAM percentage available for data fetching

	Time				m_memAccTime; //!< RAM access time
	Time				m_hddAccTime; //!< HDD access time

	virtual void StartApplication();
	virtual void StopApplication();
	/**
	 * \brief Calculates the total time to fetch the data
	 * \return the amount of time it takes to fetch all data.
	 */
	double CalculateDataFetchTime();
};

} /* namespace ns3 */

#endif /* SCRATCH_C_NODE_BLOCK_COMPUTATIONAL_LOCAL_DATA_VM_H_ */
