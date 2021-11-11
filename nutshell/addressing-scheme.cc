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
 * addressing-scheme.cc
 *
 *  Created on: Oct 19, 2015
 *      Author: ubaid
 */

#include <sstream>
#include <math.h>

#include "ns3/log.h"

#include "addressing-scheme.h"

namespace ns3 {

AddressingScheme::AddressingScheme()
{
}

void
AddressingScheme::SetNetworkAddress(std::string networkAddress, std::string subnet)
{
	ConvertAndSplit(networkAddress, subnet);
}

void
AddressingScheme::SetComponentNumber(uint32_t hosts, uint32_t netNum)
{
	m_hosts = hosts;
	m_numNet = netNum;
}

void
AddressingScheme::SetJumpValues(std::vector<uint16_t> &jumpValues)
{
	m_jumpValues = jumpValues;
}

std::string
AddressingScheme::GetNetwork(uint32_t id)
{
	return m_networks[id][0];
}
std::string
AddressingScheme::GetSubnet(uint32_t id)
{
	return m_networks[id][1];
}

std::vector< std::vector< std::string > >
AddressingScheme::GetNetworkList() const
{
	return m_networks;
}

AddressingScheme::~AddressingScheme()
{
}

void
AddressingScheme::ConvertAndSplit(std::string networkAddress, std::string subnet)
{
	std::string::size_type ipn = networkAddress.find_first_of(".");
	std::string::size_type snetn = subnet.find_first_of(".");
	int counter = 0;
	while(ipn != std::string::npos && snetn != std::string::npos)
	{
		m_oct[counter] = ConvertString(networkAddress.substr(0, ipn));
		networkAddress = networkAddress.substr(ipn+1, std::string::npos);
		m_snetP[counter] = ConvertString(subnet.substr(0, snetn));
		subnet = subnet.substr(snetn+1, std::string::npos);

		ipn = networkAddress.find_first_of(".");
		snetn = subnet.find_first_of(".");
		if(ipn == std::string::npos)
		{
			m_oct[counter+1] = ConvertString(networkAddress.substr(0, ipn));
		}
		if(snetn == std::string::npos)
		{
			m_snetP[counter+1] = ConvertString(subnet.substr(0, snetn));
		}
		counter++;
	}
}


uint16_t
AddressingScheme::ConvertString(std::string s)
{
	std::istringstream iss;
	uint16_t r;
	iss.str(s);
	iss >> r;
	return r;
}
std::string
AddressingScheme::ConvertToString(uint16_t fo, uint16_t so, uint16_t to, uint16_t foo)
{
	std::ostringstream oss;
	oss << fo <<"." << so <<"." << to << "." << foo;
	return oss.str();
}

void
AddressingScheme::ResizeNetworksVector(uint32_t numNet)
{
	m_networks.resize(numNet);
	for(uint32_t i = 0; i < numNet; i++)
	{
		m_networks[i].resize(2);
	}
}

void
AddressingScheme::CreateNetworkAddresses()
{
//	NS_LOG_UNCOND("This is parent");
	ResizeNetworksVector(m_numNet);
	uint16_t bitsReq = ceil(log2(m_hosts));

	uint16_t over = bitsReq % 8;

	uint16_t covered = floor( bitsReq / 8);

	uint16_t baseVal = pow(2, over) - 1;
	uint16_t snetPart = 255 - baseVal;
	uint16_t ipCounter[4];
	uint16_t jumpCounter = 0;

	for(int i = 0; i < 4; i++)
	{
		ipCounter[i] = m_oct[i];
	}

	uint16_t workingOct = 3 - covered;

	for(uint32_t i = 0; i < m_numNet; i++)
	{
		if(covered == 0)
		{
			m_networks[i][0] = ConvertToString(ipCounter[0], ipCounter[1], ipCounter[2], ipCounter[3]);
			m_networks[i][1] = ConvertToString(255, 255, 255, snetPart);
		}
		else if(covered == 1)
		{
			m_networks[i][0] = ConvertToString(ipCounter[0], ipCounter[1], ipCounter[2], ipCounter[3]);
			m_networks[i][1] = ConvertToString(255, 255, snetPart, 0);
		}
		else if(covered == 2)
		{
			m_networks[i][0] = ConvertToString(ipCounter[0], ipCounter[1], ipCounter[2], ipCounter[3]);
			m_networks[i][1] = ConvertToString(255, snetPart, 0, 0);
		}
		else if(covered == 3)
		{
			m_networks[i][0] = ConvertToString(ipCounter[0], ipCounter[1], ipCounter[2], ipCounter[3]);
			m_networks[i][1] = ConvertToString(snetPart, 0, 0, 0);
		}

//		NS_LOG_UNCOND("Network: " << m_networks[i][0] <<"\nSubnet:" << m_networks[i][1] << "\n");

		ipCounter[workingOct] += (baseVal+1);
//		NS_LOG_UNCOND("ip counter at working oct: " << ipCounter[workingOct]);
		if(m_jumpValues.size() > 0)
		{
			if(jumpCounter < m_jumpValues.size())
			{
				ipCounter[workingOct] += m_jumpValues[jumpCounter];
				jumpCounter++;
			}
		}
		ProcessAddress(ipCounter, workingOct);
	}
}

void
AddressingScheme::ProcessAddress(uint16_t *array, uint16_t workingOct)
{
	for(int i = (int) workingOct; i > 0; i--)
	{
//		NS_LOG_UNCOND("i : " << i <<" workingOct: " << workingOct);
		uint16_t temp = array[i] % 256;
		//NS_LOG_UNCOND("temp: " << temp);
		if(array[i] > 255 && temp == 0)
		{
			array[i] = 0;
			array[i-1]++;
		}
		else if (array[i] > 255 && temp > 0)
		{
			array[i] = temp;
			array[i-1]++;
		}
	}
}

} /* namespace ns3 */
