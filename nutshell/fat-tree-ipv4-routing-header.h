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
 * fat-tree-ipv4-routing-header.h
 *
 *  Created on: Nov 26, 2015
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#ifndef FAT_TREE_IPV4_ROUTING_HEADER_H
#define FAT_TREE_IPV4_ROUTING_HEADER_H

#include <list>

#include "ns3/header.h"
#include "ns3/packet.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"

#include "fat-tree-ipv4-rte.h"

namespace ns3 {

class FatTreeIpv4RoutingHeader: public Header
{
public:
	FatTreeIpv4RoutingHeader();
	/**
	* \brief Get the type ID.
	* \return the object TypeId
	*/
	static TypeId GetTypeId (void);

	/**
	* \brief Return the instance type identifier.
	* \return instance type ID
	*/
	virtual TypeId GetInstanceTypeId (void) const;

	virtual void Print (std::ostream& os) const;

	/**
	* \brief Get the serialized size of the packet.
	* \return size
	*/
	virtual uint32_t GetSerializedSize (void) const;

	/**
	* \brief Serialize the packet.
	* \param start Buffer iterator
	*/
	virtual void Serialize (Buffer::Iterator start) const;

	/**
	* \brief Deserialize the packet.
	* \param start Buffer iterator
	* \return size of the packet
	*/
	virtual uint32_t Deserialize (Buffer::Iterator start);
	/**
	* \brief Add a RTE to the message
	* \param rte the RTE
	*/
	void AddRte (FatTreeIpv4Rte rte);

	/**
	* \brief Clear all the RTEs from the header
	*/
	void ClearRtes ();

	/**
	* \brief Get the number of RTE included in the message
	* \returns the number of RTE in the message
	*/
	uint16_t GetRteNumber (void) const;

	/**
	* \brief Get the list of the RTEs included in the message
	* \returns the list of the RTEs in the message
	*/
	std::list<FatTreeIpv4Rte> GetRteList (void) const;

	virtual ~FatTreeIpv4RoutingHeader();
private:
	std::list<FatTreeIpv4Rte> m_rteList; //!< list of the RTEs in the message
};

/**
 * \brief Stream insertion operator.
 *
 * \param os the reference to the output stream
 * \param h the FatTreeRouting header
 * \returns the reference to the output stream
 */
std::ostream & operator << (std::ostream & os, const FatTreeIpv4RoutingHeader & h);


} /* namespace ns3 */

#endif /* FAT_TREE_IPV4_ROUTING_HEADER_H */
