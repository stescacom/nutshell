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
 * fat-tree-addressing-scheme.h
 *
 *  Created on: Nov 1, 2015
 *      Author: ubaid
 */

#ifndef NUTSHELL_FAT_TREE_ADDRESSING_SCHEME
#define NUTSHELL_FAT_TREE_ADDRESSING_SCHEME

#include "ns3/log.h"

#include "addressing-scheme.h"

namespace ns3 {
/**
 * \brief The class is responsible for creating addressing scheme
 * specific to FatTree topology
 *
 * The FatTree topology follow a specific format of addressing:
 *
 * unused.PodNumber.switchnumber.Endhost
 *
 * The addressing scheme creates IP subnets that follow the same
 * format.
 *
 */
class FatTreeAddressingScheme: public AddressingScheme {
public:
	/**
	 * \brief Class constructor
	 */
	FatTreeAddressingScheme();
	/**
	 * \brief Creates the network addresses
	 */
	virtual void CreateNetworkAddresses();
	/**
	 * \brief Sets the number of pods
	 * \param pods The number of pods
	 */
	void SetPods(uint32_t pods);
	virtual ~FatTreeAddressingScheme();

private:
	/**
	 * \brief Moves generated subnet to list
	 */
	void MoveToHolder();

	uint32_t m_pods; //!< The number of pods
	std::vector< std::vector< std::string > > m_gIpHolder; //!< The list to hold subnets.
};

} /* namespace ns3 */

#endif /* NUTSHELL_FAT_TREE_ADDRESSING_SCHEME */
