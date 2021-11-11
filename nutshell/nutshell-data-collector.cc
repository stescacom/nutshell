/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 ifast (SMC-PVT) LTD, COMSATS Institute of Information Technology & Qatar University
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
 * Authors: Ubaid ur Rahman (ubaid@ifast.pk, u.ur.rahman@gmail.com)
 *
 *
 */

#include <vector>
#include <ctime>
#include <string>
#include <sstream>

#include "ns3/log.h"
#include "nutshell-data-collector.h"

namespace ns3 {

void
NutshellDataCollector::Test(std::string t)
{
	NS_LOG_UNCOND(t);
}

NutshellDataCollector::NutshellDataCollector(void)
{
	m_timeInterval = 5;
}

NutshellDataCollector::~NutshellDataCollector()
{
}

void
NutshellDataCollector::AddDataCollector(NutshellDataCollector& newCollector)
{
	m_collectors.push_back(&newCollector);
}

void
NutshellDataCollector::AttachSinks()
{
	for(uint32_t i = 0; i < m_collectors.size(); i++)
	{
		m_collectors[i]->AttachTraceSink();
	}
}

void
NutshellDataCollector::SetTimeInterval(uint16_t t)
{
	m_timeInterval = t;
}

void
NutshellDataCollector::AttachTraceSink(){
	NS_LOG_UNCOND("parent attach trace sink");
}

void
NutshellDataCollector::Export()
{
	for(uint32_t i = 0; i < m_collectors.size(); i++)
	{
		m_collectors[i]->SetSimulationPrefix(m_simulationPrefix);
		m_collectors[i]->ExportData();
	}
}

std::string
NutshellDataCollector::MakeFileName(std::string name, std::string extension)
{
	time_t now = time(0); // get the number of seconds from 1970
	tm *ltm = localtime(&now); // gets the local time
	std::stringstream ss;
	ss << name << "_" << 1900 + ltm->tm_year << "_" << 1 + ltm->tm_mon << "_"  << ltm->tm_mday << "_"
			<< 1 + ltm->tm_hour << "_" << 1 + ltm->tm_min << "_" << 1 + ltm->tm_sec << extension;
	std::string s = ss.str();
	return s;

}

std::string
NutshellDataCollector::MakeFileName(uint32_t identifier, std::string extension)
{
	time_t now = time(0); // get the number of seconds from 1970
	tm *ltm = localtime(&now); // gets the local time
	std::stringstream ss;
	ss << identifier << "_" << 1900 + ltm->tm_year << "_" << 1 + ltm->tm_mon << "_"  << ltm->tm_mday << "_"
			<< 1 + ltm->tm_hour << "_" << 1 + ltm->tm_min << "_" << 1 + ltm->tm_sec << extension;
	std::string s = ss.str();
	return s;

}

void
NutshellDataCollector::ExportData(){
	NS_LOG_UNCOND("parent export data");
}

std::string
NutshellDataCollector::StringConcat(std::vector< std::string > s)
{
	std::stringstream ss;
	for(uint32_t i = 0; i < s.size(); i++)
	{
		ss<< s[i];
	}

	return ss.str();
}

std::string
NutshellDataCollector::StringConcat(std::string s1, std::string s2)
{
	std::stringstream ss;
	ss << s1 << s2;
	return ss.str();
}

void
NutshellDataCollector::SetSimulationPrefix(std::string s)
{
	m_simulationPrefix = s;
}


} /* namespace ns3 */
