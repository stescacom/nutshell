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
 * fat-tree-addressing-scheme.cc
 *
 *  Created on: Nov 1, 2015
 *      Author: ubaid
 */

#include <math.h>

#include "ns3/log.h"

#include "addressing-scheme.h"

#include "fat-tree-addressing-scheme.h"

namespace ns3 {

FatTreeAddressingScheme::FatTreeAddressingScheme()
{
}

void
FatTreeAddressingScheme::SetPods(uint32_t pods)
{
	m_pods = pods;
}

void
FatTreeAddressingScheme::CreateNetworkAddresses()
{
//	uint32_t podsBy2 = m_pods/2;

	for(uint32_t i = 0; i < (m_pods + 1); i++)
	{
		uint32_t jrange;
		if(i < m_pods)
		{
			jrange = m_pods;
		}
		else
		{
			uint32_t pb2 = m_pods/2;
			jrange = pow(pb2, 2);
		}
		for(uint32_t j = 0; j < jrange; j++)
		{
			m_oct[1] = i;
			m_oct[2] = j;
			m_oct[3] = 0;
			m_numNet = 1;
			m_hosts = 4;
			AddressingScheme::CreateNetworkAddresses();
			MoveToHolder();
		}
	}
	// assigning generated holder values to
	// m_networks variable
	m_networks = m_gIpHolder;

}

void
FatTreeAddressingScheme::MoveToHolder()
{
	std::vector< std::string > row;
	for(uint32_t i = 0; i < m_networks.size(); i++)
	{
		row.push_back(m_networks[i][0]);
		row.push_back(m_networks[i][1]);

		m_gIpHolder.push_back(row);
		row.clear();
	}

}

FatTreeAddressingScheme::~FatTreeAddressingScheme()
{
}

} /* namespace ns3 */
