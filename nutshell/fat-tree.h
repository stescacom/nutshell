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
 * fat-tree.h
 *
 *  Created on: Nov 2, 2015
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#ifndef NUTSHELL_FAT_TREE_H
#define NUTSHELL_FAT_TREE_H

#include <vector>

#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/node-container.h"

#include "fat-tree-config.h"
#include "fat-tree-addressing-scheme.h"
#include "access-network.h"
#include "computational-node-container.h"

namespace ns3 {
/**
 * \brief Creates a FatTree Data center Network
 *
 * The class creates a FatTree data center network architecture
 * using a configuration file provided by the user.
 *
 * \note The configuration object contains all information required by
 * each method of this class
 */
class FatTree {
public:
	/**
	 * \brief Class constructor
	 * \param config The configuration object, of FatTreeConfig class
	 */
	FatTree(FatTreeConfig config);
	virtual ~FatTree();

private:
	/**
	 * \brief Initializes the architecture creating process
	 */
	void CreateArchitecture();
	/**
	 * \brief Generates addresses for nodes according to some addressing scheme
	 */
	void GenerateAddresses();
	/**
	 * \brief Create FatTree PODs
	 */
	void CreatePods();
	/**
	 * \brief Creates all computational nodes required by the architecture
	 */
	void CreateAllNodes();
	/**
	 * \brief Creates an access network for a single POD
	 * \param podIndex The pod number
	 */
	void CreateAccessNetwork(uint32_t podIndex);
	/**
	 * \brief Creates aggregate routers for the pod
	 * \param podIndex The pod number to which the created aggregate routers will connect
	 */
	void CreateAggSw(uint32_t podIndex);
	/**
	 * \brief Creates the core pod to hold core routers
	 */
	void CreateCorePod();

	/**
	 * \brief Connects Aggregate routers to Access routers of a pod
	 * \param podIndex The pod index of which the connection is required
	 */
	void ConnectAggToAcc(uint32_t podIndex);
	/**
	 * \brief Connects core pod routers to each aggrigate router of other pod
	 */
	void ConnectCoreToPod();


//	void AddTestUdpEcho();

	/**
	 * \brief Begins the scheduling of Virtual Machines
	 */
	void BeginScheduling();

	uint32_t m_podBy2; //!< value of pod/2 (k/2)
	uint32_t m_addrCount; //!< used address counter
	FatTreeConfig m_config; //!< FatTree Configuration object
	std::vector< std::vector < AccessNetwork > > m_accessNet; //!< access network holder format: [pod][access net]
	std::vector< NodeContainer > m_aggSw; //!< aggregate routers holder format: [pod][aggregate routers]
	NodeContainer m_coreSw; //!< core routers list
	AddressingScheme * m_addrScheme; //!< address scheme for FatTree architecture
	ComputationalNodeContainer m_allNodes; //!< computational node container
	Ipv4InterfaceContainer m_nodeInterfaces; //!< list of node interfaces

	// range definer
	uint32_t m_rstart; //!< range start to group access network, aggregate routers according to k/2 while looping through the list
	uint32_t m_rend; //!< range end
};

} /* namespace ns3 */

#endif /* SCRATCH_NUTSHELL_FAT_TREE_H_ */
