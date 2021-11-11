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
 * fat-tree-ipv4-rte.h
 *
 *  Created on: Nov 26, 2015
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#ifndef FAT_TREE_IPV4_RTE_H
#define FAT_TREE_IPV4_RTE_H

#include <list>

#include "ns3/header.h"
#include "ns3/packet.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"

namespace ns3 {

class FatTreeIpv4Rte: public Header {
public:
	FatTreeIpv4Rte();
	virtual ~FatTreeIpv4Rte();

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
	   * \brief Set the prefix
	   * \param prefix the prefix
	   */
	  void SetPrefix (Ipv4Address prefix);

	  /**
	   * \brief Get the prefix
	   * \returns the prefix
	   */
	  Ipv4Address GetPrefix (void) const;

	  /**
	   * \brief Set the prefix length
	   * \param prefixLen the prefix length
	   */
	  void SetPrefixLen (uint16_t prefixLen);

	  /**
	   * \brief Get the prefix length
	   * \returns the prefix length
	   */
	  uint16_t GetPrefixLen (void) const;

private:
	Ipv4Address m_prefix;
	uint16_t m_prefixLength;
};

/**
 * \brief Stream insertion operator.
 *
 * \param os the reference to the output stream
 * \param h the Routing Table Entry
 * \returns the reference to the output stream
 */
std::ostream & operator << (std::ostream & os, const FatTreeIpv4Rte & h);

} /* namespace ns3 */

#endif /* SCRATCH_FATTREEROUTINGTEST_FAT_TREE_IPV4_RTE_H_ */
