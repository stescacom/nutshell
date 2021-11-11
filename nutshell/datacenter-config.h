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
 * datacenter-config.h
 *
 *  Created on: Oct 19, 2015
 *      Author: ubaid
 */

#ifndef DATACENTER_CONFIG
#define DATACENTER_CONFIG



#include "ns3/nstime.h"
#include "ns3/callback.h"


#include "processing-power-util.h"
#include "storage-util.h"
#include "application-size-util.h"

#include "nutshell-data-collector.h"


namespace ns3 {
/**
 * \brief The class defines generic properties of datacenter
 * to be configured by user.
 *
 * This class is an interface between the Architecture creating
 * classes and User requirement. The class define generic
 * configuration data structure to hold the user requirement.
 * The architecture creating classes use configuration
 * object of this or its child class to create the
 * datacenter architecture.
 */
class DatacenterConfig {
public:
	/**
	 * \brief ENUM to define the distribution
	 */
	enum Procedure_e {
		RANDOM
	};
	/**
	 * \brief Class constructor
	 */
	DatacenterConfig();
	/**
	 * \brief Configures Node with different minimum and maximum values
	 * for resources
	 * \param pmin The minimum processing power
	 * \param pmax The maximum processing power
	 * \param psmin The minimum primary storage
	 * \param psmax The maximum primary storage
	 * \param ssmin The minimum secondary storage
	 * \param ssmax The maximum secondary storage
	 */
	void ConfigureNode(const std::string pmin,
						const std::string pmax,
						const std::string psmin,
						const std::string psmax,
						const std::string ssmin,
						const std::string ssmax);
	/**
	 * \brief Configures Node with same resources i.e homogeneous resources
	 * \param processing The processing power
	 * \param primary The primary storage
	 * \param secondary The secondary storage
	 */
	void ConfigureNode(const std::string processing,
							const std::string primary,
							const std::string secondary);
	/**
	 * \brief Configures VM general properties with different minimum and maximum values
	 * for resources and application size
	 * \param vmNum The number of VM to create
	 * \param pmin The minimum processing power
	 * \param pmax The maximum processing power
	 * \param psmin The minimum primary storage
	 * \param psmax The maximum primary storage
	 * \param ssmin The minimum secondary storage
	 * \param ssmax The maximum secondary storage
	 * \param appSizeMin The minimum application size
	 * \param appSizeMax The maximum application size
	 */
	void ConfigureVmGeneral(const uint32_t vmNum,
							const std::string pmin,
							const std::string pmax,
							const std::string psmin,
							const std::string psmax,
							const std::string ssmin,
							const std::string ssmax,
							const std::string appSizeMin,
							const std::string appSizeMax);
	/**
	 * \brief Configures the VM data requirement with different minimum and maximum values
	 * \param require The flag to set if VMs require data
	 * \param numOfVmWithServerDataSrc The number of VMs to have Server for data source
	 * \param distribution The distribution
	 * \param dAmountMin The minimum data amount
	 * \param dAmountMax The maximum data amount
	 * \param hddRwRateMin The minimum read/write rate of HDD
	 * \param hddRwRateMax The maximum read/write rate of HDD
	 * \param memRwRateMin The minimum read/write rate of RAM
	 * \param memRwRateMax The maximum read/write rate of RAM
	 * \param numOfProcAccessMin The minimum number of data access by processor
	 * \param numOfProcAccessMax The maximum number of data access by processor
	 * \param memPdfMin The minimum percentage of Memory for data fetching
	 * \param memPdfMax The maximum percentage of Memory for data fetching
	 * \param hddMinAccT The minimum HDD access time
	 * \param hddMaxAccT The maximum HDD access time
	 * \param memMinAccT The minimum memory access time
	 * \param memMaxAccT The maximum memory access time
	 */
	void ConfigureVmDataReq(bool require, uint32_t numOfVmWithServerDataSrc,
							Procedure_e distribution,
							std::string dAmountMin, std::string dAmountMax,
							std::string hddRwRateMin, std::string hddRwRateMax,
							std::string memRwRateMin, std::string memRwRateMax,
							uint32_t numOfProcAccessMin, uint32_t numOfProcAccessMax,
							double memPdfMin, double memPdfMax,
							Time hddMinAccT, Time hddMaxAccT,
							Time memMinAccT, Time memMaxAccT);
	/**
	 * \brief Configure VM split, to perform VM execution on different
	 * Nodes in case not enough resources are available.
	 *
	 * \param enable The flag to enable VM splitting
	 * \param ratio The ratio at which the VM is supposed to be split.
	 */
	void ConfigureVmSplits(bool enable, std::string ratio);
	/**
	 * \brief Configure VM arrival time with different values
	 * \param min The minimum time of VM arrival
	 * \param max The maximum time of VM arrival
	 *
	 */
	void ConfigureVmArrivals(Time min, Time max);

	/**
	 * \brief Configure VM Network properties with different values
	 * \param mtu The Maximum Transmission Unit
	 * \param protocolType The protocol to use for communication e.g ns3::TcpSocketFactory
	 * \param vmTransRateMin The minimum transmission rate at which the VM transmit/receive data
	 * \param vmTransRateMax The maximum transmission rate at which the VM transmit/receive data
	 */
	void ConfigureVmNetwork (uint32_t mtu, std::string protocolType,
									std::string vmTransRateMin,
									std::string vmTransRateMax);
	/**
	 * \brief Configure VM general properties with similar resource requirement
	 * \param vmNum The number of VM to create
	 * \param processing The processing power requirement
	 * \param primary The primary storage requirement
	 * \param secondary The secondary storage requirement
	 * \param appSize The application size of VM
	 */
	void ConfigureVmGeneral(const uint32_t vmNum,
							const std::string processing,
							const std::string primary,
							const std::string secondary,
							const std::string appSize);
	/**
	 * \brief Configures the VM data requirement with similar values
	 * \param require The flag to set if VMs require data
	 * \param numOfVmWithServerDataSrc The number of VMs to have Server for data source
	 * \param distribution The distribution
	 * \param dAmount The data amount
	 * \param hddRwRate The read/write rate of HDD
	 * \param memRwRate The read/write rate of RAM
	 * \param numOfProcAccess The number of data access by processor
	 * \param memPdf The percentage of Memory for data fetching
	 * \param hddAccT The HDD access time
	 * \param memAccT The memory access time
	 */
	void ConfigureVmDataReq(bool require, uint32_t numOfVmWithServerDataSrc,
							Procedure_e distribution,
							std::string dAmount,
							std::string hddRwRate,
							std::string memRwRate,
							uint32_t numOfProcAccess,
							double memPdf,
							Time hddAccT,
							Time memAccT);
	/**
	 * \brief Configure VM arrival time with similar values
	 * \param t The time of VM arrival
	 *
	 */
	void ConfigureVmArrivals(Time t);
	/**
	 * \brief Configure VM Network properties with similar values
	 * \param mtu The Maximum Transmission Unit
	 * \param protocolType The protocol to use for communication e.g ns3::TcpSocketFactory
	 * \param vmTransRate The transmission rate at which the VM transmit/receive data
	 */
	void ConfigureVmNetwork (uint32_t mtu, std::string protocolType,
								std::string vmTransRate);
	/**
	 * \brief Configure the number of Storage server
	 * \param numOfServ Then number of storage server to create.
	 */
	void ConfigureStorageServer(uint32_t numOfServ);
	/**
	 * \brief Enable tracing to capture network data
	 * \param collector The data collector object containing list of sub collector.
	 */
	void EnableTracing( NutshellDataCollector& collector);
	/**
	 * \brief Get the node minimum processing resource value
	 * \return The processing string value
	 */
	std::string GetNodeMinProcessing() const;
	/**
	 * \brief Get the node maximum processing resource value
	 * \return The processing power string value
	 */
	std::string GetNodeMaxProcessing() const;
	/**
	 * \brief Get the node minimum primary storage resource value
	 * \return The primary storage string value
	 */
	std::string GetNodeMinPrimaryStorage() const;
	/**
	 * \brief Get the node maximum primary storage resource value
	 * \return The primary storage string value
	 */
	std::string GetNodeMaxPrimaryStorage() const;
	/**
	 * \brief Get the node minimum secondary storage resource value
	 * \return The secondary storage string value
	 */
	std::string GetNodeMinSecondaryStorage() const;
	/**
	 * \brief Get the node maximum secondary storage resource value
	 * \return The secondary storage string value
	 */
	std::string GetNodeMaxSecondaryStorage() const;
	/**
	 * \brief Get VM minimum processing requirement
	 * \return The processing requirement string value
	 */
	std::string GetVmMinProcessing() const;
	/**
	 * \brief Get VM maximum processing requirement
	 * \return The processing requirement string value
	 */
	std::string GetVmMaxProcessing() const;
	/**
	 * \brief Get VM minimum primary storage requirement
	 * \return The primary storage requirement string value
	 */
	std::string GetVmMinPrimaryStorage() const;
	/**
	 * \brief Get VM maximum primary storage requirement
	 * \return The primary storage requirement string value
	 */
	std::string GetVmMaxPrimaryStorage() const;
	/**
	 * \brief Get VM minimum secondary storage requirement
	 * \return The secondary storage requirement string value
	 */
	std::string GetVmMinSecondaryStorage() const;
	/**
	 * \brief Get VM maximum secondary storage requirement
	 * \return The secondary storage requirement string value
	 */
	std::string GetVmMaxSecondaryStorage() const;
	/**
	 * \brief Get VM minimum application size
	 * \return The application size string value
	 */
	std::string GetVmMinAppSize() const;
	/**
	 * \brief Get VM maximum application size
	 * \return The application size string value
	 */
	std::string GetVmMaxAppSize() const;
	/**
	 * \brief Get the number of VMs to create
	 * \return The number of VM
	 */
	uint32_t	GetNumOfVmToCreate() const;

	/**
	 * \brief Check if VM required data
	 * \return The boolean value of VM required data flag
	 */
	bool		IsVmRequiredData() const;
	/**
	 * \brief Get minimum VM data amount
	 * \return The data amount string value
	 */
	std::string GetVmDataAmountMin() const;
	/**
	 * \brief Get maximum VM data amount
	 * \return The data amount string value
	 */
	std::string	GetVmDataAmountMax() const;
	/**
	 * \brief Get minimum VM HDD read/write rate
	 * \return The HDD read/write rate string value
	 */
	std::string	GetVmHddRwRateMin() const;
	/**
	 * \brief Get maximum VM HDD read/write rate
	 * \return The HDD read/write rate string value
	 */
	std::string	GetVmHddRwRateMax() const;
	/**
	 * \brief Get minimum VM memory read/write rate
	 * \return The memory read/write rate string value
	 */
	std::string GetVmMemRwRateMin() const;
	/**
	 * \brief Get maximum VM memory read/write rate
	 * \return The memory read/write rate string value
	 */
	std::string GetVmMemRwRateMax() const;
	/**
	 * \brief Get minimum number of processor access to memory
	 * \return The number of processor access
	 */
	uint32_t	GetVmNumOfProcAccessMin() const;
	/**
	 * \brief Get maximum number of processor access to memory
	 * \return The number of processor access
	 */
	uint32_t	GetVmNumOfProcAccessMax() const;
	/**
	 * \brief Get minimum percentage of memory for data fetching
	 * \return The percentage of memory
	 */
	double		GetVmMemPdfMin() const;
	/**
	 * \brief Get maximum percentage of memory for data fetching
	 * \return The percentage of memory
	 */
	double		GetVmMemPdfMax() const;
	/**
	 * \brief Get minimum VM HDD access time
	 * \return The access time
	 */
	Time		GetVmHddMinAccessTime() const;
	/**
	 * \brief Get maximum VM HDD access time
	 * \return The access time
	 */
	Time		GetVmHddMaxAccessTime() const;
	/**
	 * \brief Get minimum VM memory access time
	 * \return The access time
	 */
	Time		GetVmMemMinAccessTime() const;
	/**
	 * \brief Get maximum VM memory access time
	 * \return The access time
	 */
	Time		GetVmMemMaxAccessTime() const;
	/**
	 * \brief Get the number of VMs with storage server as data source
	 * \return The number of VMs
	 */
	uint32_t	GetNumOfVmWithServerDataSrc() const;
	/**
	 * \brief Get the distribution type
	 * \return The distribution procedure type
	 */
	Procedure_e	GetVmDistributionType() const;
	/**
	 * \brief Check if VM split is allowed
	 * \return The boolean value of split allowed flag
	 */
	bool		IsVmSplitAllowed() const;
	/**
	 * \brief Get the VM split ratio
	 * \return The ratio string value
	 */
	std::string	GetVmSplitRatio() const;

	/**
	 * \brief Get the minimum VM arrival time
	 * \return The arrival time
	 */
	Time		GetVmArrivalTimeMin() const;
	/**
	 * \brief Get the maximum VM arrival time
	 * \return The arrival time
	 */
	Time		GetVmArrivalTimeMax()	const;
	/**
	 * \brief Get the VM MTU size
	 * \return The MTU size
	 */
	uint32_t	GetVmMtu() const;
	/**
	 * \brief Get minimum VM transmission rate
	 * \return The transmission rate string value
	 */
	std::string	GetVmTransmissionRateMin() const;
	/**
	 * \brief Get maximum VM transmission rate
	 * \return The transmission rate string value
	 */
	std::string	GetVmTransmissionRateMax() const;
	/**
	 * \brief Get the VM protocol type
	 * \return The VM protocol type string value
	 */
	std::string GetVmProtocolType() const;

	/**
	 * \brief Get the number of storage servers
	 * \return the number of storage server.
	 */
	uint32_t	GetNumOfStorageServer() const;

	/**
	 * \brief Checks if tracing is enabled
	 * \return The boolean value of tracing flag
	 */
	bool		IsTracingEnabled() const;
	/**
	 * \brief Get the data collector object
	 * \return The Data collector object.
	 */
	NutshellDataCollector& GetDataCollector();

	virtual ~DatacenterConfig();

protected:
	/**
	 * \brief Structure for Node Configuration
	 */
	struct NodeConfig {
		std::string 		processingMin, processingMax;
		std::string			primaryStorageMin, primaryStorageMax;
		std::string			secondaryStorageMin, secondaryStorageMax;
	};
	/**
	 * \brief Structure for VM Configuration
	 */
	struct VmConfig {
		/* ------------ General ---------*/
		std::string 		processingMin, processingMax;
		std::string			primaryStorageMin, primaryStorageMax;
		std::string			secondaryStorageMin, secondaryStorageMax;
		std::string			appSizeMin, appSizeMax;
		uint32_t 			numOfVmsToCreate;

		/* ------------ Data ---------*/
		bool				requireData;
		std::string			dataAmountMin, dataAmountMax;
		std::string			hddRwRateMin, hddRwRateMax, memRwRateMin, memRwRateMax;
		uint32_t			numOfProcAccessMin, numOfProcAccessMax;
		double				memPDFMin, memPDFMax;
		Time				hddMinAccessTime, hddMaxAccessTime, memMinAccessTime, memMaxAccessTime;

		uint32_t			numOfVmWithServerDataSource;
		Procedure_e			distribution;

		/* ------------ Split ---------*/
		bool				allowVmSplit;
		std::string			splitRatio;

		/* ------------ Arrival ---------*/
		Time				arrivalTimeMin, arrivalTimeMax;

		/* ----------- Network --------- */
		uint32_t			mtu;
		std::string			vmTransmissionRateMin, vmTransmissionRateMax;
		std::string			tid;
	};
	/**
	 * \brief Structure for Storage Server Configuration
	 */
	struct StorageServerConfig {
		uint32_t			numOfServers;
	};

	NodeConfig 	m_nodeConfiguration; //!< The node configuration
	VmConfig	m_vmConfiguration; //!< The VM configuration
	StorageServerConfig m_storageServer; //!< The storage server configuration

	bool m_enableTracing; //!< The tracing flag
	NutshellDataCollector m_dataCollector; //!< The data collector
};

} /* namespace ns3 */

#endif /* DATACENTER_CONFIG */
