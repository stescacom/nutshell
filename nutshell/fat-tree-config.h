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
 * fat-tree-config.h
 *
 *  Created on: Nov 2, 2015
 *      Author: ubaid
 */

#ifndef NUTSHELL_FAT_TREE_CONFIG_H
#define NUTSHELL_FAT_TREE_CONFIG_H

#include "ns3/ipv4-routing-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/point-to-point-helper.h"

#include "datacenter-config.h"
#include "addressing-scheme.h"
#include "fat-tree-addressing-scheme.h"
#include "vm-scheduler.h"

namespace ns3 {
/**
 * \brief This class defines properties to configure
 * the FatTree topology
 *
 * This class contains specialized properties related
 * to FatTree topology, that guide the creation of topology
 *
 */
class FatTreeConfig: public DatacenterConfig {
public:
	/**
	 * \brief Class constructor
	 */
	FatTreeConfig();

	/**
	 * \brief Set the number of Pods
	 * \param pods The number of pods
	 */
	void SetPods(uint32_t pods);
	/**
	 * \brief Set base network, from which subnets for each
	 * Pod is created.
	 * \param netAddr The network address string value
	 * \param subnet The subnet mask
	 */
	void SetBaseNetwork(std::string netAddr, std::string subnet);
	/**
	 * \brief Set the link that is used to connect two devices
	 * \param p2p point-to-point channel
	 */
	void SetLink(PointToPointHelper p2p);
	/**
	 * \brief Set custom routing protocol to be used by the topology
	 * \param customRouting The custom routing reference
	 */
	void SetRouting(Ipv4RoutingHelper & customRouting);
	/**
	 * \brief Set a custom addressing scheme to be used by the topology
	 * \param addressing The addressing scheme reference
	 */
	void SetAddressingScheme(AddressingScheme & addressing);
	/**
	 * \brief Set the Internet stack to be installed on each device
	 * \param Internet The Internet stack
	 */
	void SetInternetStack(InternetStackHelper internet);
	/**
	 * \brief Set a custom VM scheduler
	 * \param sch The VM scheduler reference
	 */
	void SetVmScheuler(VmScheduler & sch);

	/**
	 * \brief Get the number of Pods
	 * \return The number of Pod
	 */
	uint32_t GetPods() const;
	/**
	 * \brief Get the channel used between two devices
	 * \return The point-to-point link
	 */
	PointToPointHelper GetLink() const;
	/**
	 * \brief Get the routing protocol
	 * \return The routing protocol pointer
	 */
	Ipv4RoutingHelper * GetRouting() const;
	/**
	 * \brief Get the Addressing scheme
	 * \return The addressing scheme pointer
	 */
	AddressingScheme * GetAddressingScheme();
	/**
	 * \brief Get the VM scheduler
	 * \return The VM scheduler pointer
	 */
	VmScheduler * GetVmScheduler() const;
	/**
	 * \brief Get the Internet Stack
	 * \return The Internet Stack
	 */
	InternetStackHelper GetStack() const;
	/**
	 * \brief Get the Network Address
	 * \return The main network IP string
	 */
	std::string GetNetworkAddress() const;
	/**
	 * \brief Get the Network Subnet Mask
	 * \return The main subnet mask
	 */
	std::string GetSubnet() const;

	virtual ~FatTreeConfig();

private:

	uint32_t m_pods; //!< The number of Pods to create

	std::string m_baseNetowrkAddress; //!< The base network IP address
	std::string m_baseSubnet; //!< The base network Subnet Mask
	PointToPointHelper m_p2pLink; //!< The point-to-point link between devices
	InternetStackHelper m_stack; //!< The Internet stack
	Ipv4RoutingHelper* m_customRouting; //!< Pointer to custom routing protocol
	AddressingScheme* m_addressing; //!< Pointer to custom addressing scheme

	FatTreeAddressingScheme m_defaultAddressing; //!< FatTree default addressing scheme

	VmScheduler * m_customVmScheduler; //!< Pointer to custom VM scheduler


};

} /* namespace ns3 */

#endif /* SCRATCH_NUTSHELL_FAT_TREE_CONFIG_H_ */
