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
 * datacenter-config.cc
 *
 *  Created on: Oct 19, 2015
 *      Author: ubaid
 */


#include "datacenter-config.h"


#include "nutshell-data-collector.h"


#include "ns3/callback.h"

namespace ns3 {

DatacenterConfig::DatacenterConfig()
{
	m_vmConfiguration.numOfVmWithServerDataSource = 0;
	m_storageServer.numOfServers = 0;
	m_enableTracing = false;
	m_vmConfiguration.requireData = false;
//	m_dataCollector = new NutshellDataCollector();
}

void
DatacenterConfig::ConfigureNode(const std::string pmin,
								const std::string pmax,
								const std::string psmin,
								const std::string psmax,
								const std::string ssmin,
								const std::string ssmax)
{
	m_nodeConfiguration.processingMin = pmin;
	m_nodeConfiguration.processingMax = pmax;
	m_nodeConfiguration.primaryStorageMin = psmin;
	m_nodeConfiguration.primaryStorageMax = psmax;
	m_nodeConfiguration.secondaryStorageMin = ssmin;
	m_nodeConfiguration.secondaryStorageMax = ssmax;
}

void
DatacenterConfig::ConfigureNode(const std::string processing,
								const std::string primary,
								const std::string secondary)
{
	m_nodeConfiguration.processingMax = m_nodeConfiguration.processingMin = processing;
	m_nodeConfiguration.primaryStorageMax = m_nodeConfiguration.primaryStorageMin = primary;
	m_nodeConfiguration.secondaryStorageMax = m_nodeConfiguration.secondaryStorageMin = secondary;
}

void
DatacenterConfig::ConfigureVmGeneral(const uint32_t vmNum,
										const std::string pmin,
										const std::string pmax,
										const std::string psmin,
										const std::string psmax,
										const std::string ssmin,
										const std::string ssmax,
										const std::string appSizeMin,
										const std::string appSizeMax)
{
	m_vmConfiguration.numOfVmsToCreate = vmNum;
	m_vmConfiguration.processingMin = pmin;
	m_vmConfiguration.processingMax = pmax;
	m_vmConfiguration.primaryStorageMin = psmin;
	m_vmConfiguration.primaryStorageMax = psmax;
	m_vmConfiguration.secondaryStorageMin = ssmin;
	m_vmConfiguration.secondaryStorageMax = ssmax;
	m_vmConfiguration.appSizeMin = appSizeMin;
	m_vmConfiguration.appSizeMax = appSizeMax;
}

void
DatacenterConfig::ConfigureVmDataReq(bool require, uint32_t numOfVmWithServerDataSrc,
						Procedure_e distribution,
						std::string dAmountMin, std::string dAmountMax,
						std::string hddRwRateMin, std::string hddRwRateMax,
						std::string memRwRateMin, std::string memRwRateMax,
						uint32_t numOfProcAccessMin, uint32_t numOfProcAccessMax,
						double memPdfMin, double memPdfMax,
						Time hddMinAccT, Time hddMaxAccT,
						Time memMinAccT, Time memMaxAccT)
{
	m_vmConfiguration.requireData = require;
	m_vmConfiguration.numOfVmWithServerDataSource = numOfVmWithServerDataSrc;
	m_vmConfiguration.distribution = distribution;
	m_vmConfiguration.dataAmountMin = dAmountMin;
	m_vmConfiguration.dataAmountMax = dAmountMax;
	m_vmConfiguration.hddRwRateMin = hddRwRateMin;
	m_vmConfiguration.hddRwRateMax = hddRwRateMax;
	m_vmConfiguration.memRwRateMin = memRwRateMin;
	m_vmConfiguration.memRwRateMax = memRwRateMax;
	m_vmConfiguration.numOfProcAccessMin = numOfProcAccessMin;
	m_vmConfiguration.numOfProcAccessMax = numOfProcAccessMax;
	m_vmConfiguration.memPDFMin = memPdfMin;
	m_vmConfiguration.memPDFMax = memPdfMax;
	m_vmConfiguration.hddMinAccessTime = hddMinAccT;
	m_vmConfiguration.hddMaxAccessTime = hddMaxAccT;
	m_vmConfiguration.memMinAccessTime = memMinAccT;
	m_vmConfiguration.memMaxAccessTime = memMaxAccT;
}

void
DatacenterConfig::ConfigureVmSplits(bool enable, std::string ratio)
{
	m_vmConfiguration.allowVmSplit = enable;
	m_vmConfiguration.splitRatio = ratio;
}
void
DatacenterConfig::ConfigureVmArrivals(Time min, Time max)
{
	m_vmConfiguration.arrivalTimeMin = min;
	m_vmConfiguration.arrivalTimeMax = max;
}

void
DatacenterConfig::ConfigureVmNetwork (uint32_t mtu, std::string protocolType,
									std::string vmTransRateMin,
									std::string vmTransRateMax)
{
	m_vmConfiguration.mtu = mtu;
	m_vmConfiguration.tid = protocolType;
	m_vmConfiguration.vmTransmissionRateMin = vmTransRateMin;
	m_vmConfiguration.vmTransmissionRateMax = vmTransRateMax;
}

void
DatacenterConfig::ConfigureVmGeneral(const uint32_t vmNum,
										const std::string processing,
										const std::string primary,
										const std::string secondary,
										const std::string appSize)
{
	m_vmConfiguration.numOfVmsToCreate = vmNum;
	m_vmConfiguration.processingMax = m_vmConfiguration.processingMin = processing;
	m_vmConfiguration.primaryStorageMax = m_vmConfiguration.primaryStorageMin = primary;
	m_vmConfiguration.secondaryStorageMax = m_vmConfiguration.secondaryStorageMin = secondary;
	m_vmConfiguration.appSizeMax = m_vmConfiguration.appSizeMin = appSize;
}

void
DatacenterConfig::ConfigureVmDataReq(bool require, uint32_t numOfVmWithServerDataSrc,
										Procedure_e distribution,
										std::string dAmount,
										std::string hddRwRate,
										std::string memRwRate,
										uint32_t numOfProcAccess,
										double memPdf,
										Time hddAccT,
										Time memAccT)
{
	m_vmConfiguration.requireData = require;
	m_vmConfiguration.numOfVmWithServerDataSource = numOfVmWithServerDataSrc;
	m_vmConfiguration.distribution = distribution;
	m_vmConfiguration.dataAmountMin = m_vmConfiguration.dataAmountMax = dAmount;
	m_vmConfiguration.hddRwRateMin = m_vmConfiguration.hddRwRateMax = hddRwRate;
	m_vmConfiguration.memRwRateMin = m_vmConfiguration.memRwRateMax = memRwRate;
	m_vmConfiguration.numOfProcAccessMin = m_vmConfiguration.numOfProcAccessMax = numOfProcAccess;
	m_vmConfiguration.memPDFMin = m_vmConfiguration.memPDFMax = memPdf;
	m_vmConfiguration.hddMinAccessTime = m_vmConfiguration.hddMaxAccessTime = hddAccT;
	m_vmConfiguration.memMinAccessTime = m_vmConfiguration.memMaxAccessTime = memAccT;
}

void
DatacenterConfig::ConfigureVmArrivals(Time t)
{
	m_vmConfiguration.arrivalTimeMin = m_vmConfiguration.arrivalTimeMax = t;
}

void
DatacenterConfig::ConfigureVmNetwork (uint32_t mtu, std::string protocolType,
								std::string vmTransRate)
{
	m_vmConfiguration.mtu = mtu;
	m_vmConfiguration.tid = protocolType;
	m_vmConfiguration.vmTransmissionRateMin = m_vmConfiguration.vmTransmissionRateMax = vmTransRate;
}


void
DatacenterConfig::ConfigureStorageServer(uint32_t numOfServ)
{
	m_storageServer.numOfServers = numOfServ;
}

void
DatacenterConfig::EnableTracing(NutshellDataCollector& collector)
{
	m_enableTracing = true;
	m_dataCollector = collector;
}

std::string
DatacenterConfig::GetNodeMinProcessing() const
{
	return m_nodeConfiguration.processingMin;
}
std::string
DatacenterConfig::GetNodeMaxProcessing() const
{
	return m_nodeConfiguration.processingMax;
}
std::string
DatacenterConfig::GetNodeMinPrimaryStorage() const
{
	return m_nodeConfiguration.primaryStorageMin;
}
std::string
DatacenterConfig::GetNodeMaxPrimaryStorage() const
{
	return m_nodeConfiguration.primaryStorageMax;
}
std::string
DatacenterConfig::GetNodeMinSecondaryStorage() const
{
	return m_nodeConfiguration.secondaryStorageMin;
}
std::string
DatacenterConfig::GetNodeMaxSecondaryStorage() const
{
	return m_nodeConfiguration.secondaryStorageMax;
}

std::string
DatacenterConfig::GetVmMinProcessing() const
{
	return m_vmConfiguration.processingMin;
}
std::string
DatacenterConfig::GetVmMaxProcessing() const
{
	return m_vmConfiguration.processingMax;
}
std::string
DatacenterConfig::GetVmMinPrimaryStorage() const
{
	return m_vmConfiguration.primaryStorageMin;
}
std::string
DatacenterConfig::GetVmMaxPrimaryStorage() const
{
	return m_vmConfiguration.primaryStorageMax;
}
std::string DatacenterConfig::GetVmMinSecondaryStorage() const
{
	return m_vmConfiguration.secondaryStorageMax;
}
std::string DatacenterConfig::GetVmMaxSecondaryStorage() const
{
	return m_vmConfiguration.secondaryStorageMax;
}
std::string
DatacenterConfig::GetVmMinAppSize() const
{
	return m_vmConfiguration.appSizeMin;
}
std::string
DatacenterConfig::GetVmMaxAppSize() const
{
	return m_vmConfiguration.appSizeMax;
}

uint32_t
DatacenterConfig::GetNumOfVmToCreate() const
{
	return m_vmConfiguration.numOfVmsToCreate;
}

bool
DatacenterConfig::IsVmRequiredData() const
{
	return m_vmConfiguration.requireData;
}
std::string
DatacenterConfig::GetVmDataAmountMin() const
{
	return m_vmConfiguration.dataAmountMin;
}
std::string
DatacenterConfig::GetVmDataAmountMax() const
{
	return m_vmConfiguration.dataAmountMax;
}
std::string
DatacenterConfig::GetVmHddRwRateMin() const
{
	return m_vmConfiguration.hddRwRateMin;
}

std::string
DatacenterConfig::GetVmHddRwRateMax() const
{
	return m_vmConfiguration.hddRwRateMax;
}

std::string
DatacenterConfig::GetVmMemRwRateMin() const
{
	return m_vmConfiguration.memRwRateMin;
}

std::string
DatacenterConfig::GetVmMemRwRateMax() const
{
	return m_vmConfiguration.memRwRateMax;
}

uint32_t
DatacenterConfig::GetVmNumOfProcAccessMin() const
{
	return m_vmConfiguration.numOfProcAccessMin;
}
uint32_t
DatacenterConfig::GetVmNumOfProcAccessMax() const
{
	return m_vmConfiguration.numOfProcAccessMax;
}

double
DatacenterConfig::GetVmMemPdfMin() const
{
	return m_vmConfiguration.memPDFMin;
}

double
DatacenterConfig::GetVmMemPdfMax() const
{
	return m_vmConfiguration.memPDFMax;
}

Time
DatacenterConfig::GetVmHddMinAccessTime() const
{
	return m_vmConfiguration.hddMinAccessTime;
}

Time
DatacenterConfig::GetVmHddMaxAccessTime() const
{
	return m_vmConfiguration.hddMaxAccessTime;
}

Time
DatacenterConfig::GetVmMemMinAccessTime() const
{
	return m_vmConfiguration.memMinAccessTime;
}

Time
DatacenterConfig::GetVmMemMaxAccessTime() const
{
	return m_vmConfiguration.memMaxAccessTime;
}

uint32_t
DatacenterConfig::GetNumOfVmWithServerDataSrc() const
{
	return m_vmConfiguration.numOfVmWithServerDataSource;
}
DatacenterConfig::Procedure_e
DatacenterConfig::GetVmDistributionType() const
{
	return m_vmConfiguration.distribution;
}

bool
DatacenterConfig::IsVmSplitAllowed() const
{
	return m_vmConfiguration.allowVmSplit;
}

std::string
DatacenterConfig::GetVmSplitRatio() const
{
	return m_vmConfiguration.splitRatio;
}

Time
DatacenterConfig::GetVmArrivalTimeMin() const
{
	return m_vmConfiguration.arrivalTimeMin;
}
Time
DatacenterConfig::GetVmArrivalTimeMax()	const
{
	return m_vmConfiguration.arrivalTimeMax;
}

uint32_t
DatacenterConfig::GetVmMtu() const
{
	return m_vmConfiguration.mtu;
}

std::string
DatacenterConfig::GetVmTransmissionRateMin() const
{
	return m_vmConfiguration.vmTransmissionRateMin;
}

std::string
DatacenterConfig::GetVmTransmissionRateMax() const
{
	return m_vmConfiguration.vmTransmissionRateMax;
}

std::string
DatacenterConfig::GetVmProtocolType() const
{
	return m_vmConfiguration.tid;
}

uint32_t
DatacenterConfig::GetNumOfStorageServer() const
{
	return m_storageServer.numOfServers;
}

bool
DatacenterConfig::IsTracingEnabled() const
{
	return m_enableTracing;
}

NutshellDataCollector&
DatacenterConfig::GetDataCollector()
{
	return m_dataCollector;
}

DatacenterConfig::~DatacenterConfig()
{
}

} /* namespace ns3 */
