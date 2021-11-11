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
 * fat-tree-ipv4-rte.cc
 *
 *  Created on: Nov 26, 2015
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#include <list>

#include "ns3/header.h"
#include "ns3/packet.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"
#include "ns3/log.h"

#include "fat-tree-ipv4-rte.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(FatTreeIpv4Rte);

FatTreeIpv4Rte::FatTreeIpv4Rte()
{
}

TypeId
FatTreeIpv4Rte::GetTypeId()
{
	static TypeId tid = TypeId ("ns3::FatTreeIpv4Rte")
			.SetParent<Header>()
			.AddConstructor<FatTreeIpv4Rte>();
	return tid;
}

TypeId
FatTreeIpv4Rte::GetInstanceTypeId() const
{
	return GetTypeId();
}

void
FatTreeIpv4Rte::Print (std::ostream & os) const
{
  os << "prefix " << m_prefix << "/" << int(m_prefixLength);
}

uint32_t
FatTreeIpv4Rte::GetSerializedSize () const
{
  return 6;
}

void
FatTreeIpv4Rte::Serialize (Buffer::Iterator i) const
{
  uint8_t tmp[4];

  m_prefix.Serialize (tmp);


  i.Write (tmp, 4);

  i.WriteU16 (m_prefixLength);
}

uint32_t
FatTreeIpv4Rte::Deserialize (Buffer::Iterator i)
{
  uint8_t tmp[4];

  i.Read (tmp, 4);

//  m_prefix.Set(*tmp);
  m_prefix = m_prefix.Deserialize(tmp);
  m_prefixLength = i.ReadU16 ();


  return GetSerializedSize ();
}

void
FatTreeIpv4Rte::SetPrefix (Ipv4Address prefix)
{
  m_prefix = prefix;
}

Ipv4Address FatTreeIpv4Rte::GetPrefix () const
{
  return m_prefix;
}

void
FatTreeIpv4Rte::SetPrefixLen (uint16_t prefixLen)
{
  m_prefixLength = prefixLen;
}

uint16_t
FatTreeIpv4Rte::GetPrefixLen () const
{
  return m_prefixLength;
}

std::ostream & operator << (std::ostream & os, const FatTreeIpv4Rte & h)
{
  h.Print (os);
  return os;
}

FatTreeIpv4Rte::~FatTreeIpv4Rte()
{
}

} /* namespace ns3 */
