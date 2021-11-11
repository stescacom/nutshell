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
 * fat-tree-ipv4-routing-header.cc
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

#include "fat-tree-ipv4-routing-header.h"
#include "fat-tree-ipv4-rte.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED(FatTreeIpv4RoutingHeader);

FatTreeIpv4RoutingHeader::FatTreeIpv4RoutingHeader()
{
}

TypeId FatTreeIpv4RoutingHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::FatTreeIpv4RoutingHeader")
    .SetParent<Header> ()
    .SetGroupName ("Internet")
    .AddConstructor<FatTreeIpv4RoutingHeader> ();
  return tid;
}

TypeId FatTreeIpv4RoutingHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void FatTreeIpv4RoutingHeader::Print (std::ostream & os) const
{
  for (std::list<FatTreeIpv4Rte>::const_iterator iter = m_rteList.begin ();
      iter != m_rteList.end (); iter ++)
    {
      os << " | ";
      iter->Print (os);
    }
}

uint32_t FatTreeIpv4RoutingHeader::GetSerializedSize () const
{
  FatTreeIpv4Rte rte;
  return 3 + m_rteList.size () * rte.GetSerializedSize ();
}

void FatTreeIpv4RoutingHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteU8 (1);
  i.WriteU16 (0);

  for (std::list<FatTreeIpv4Rte>::const_iterator iter = m_rteList.begin ();
      iter != m_rteList.end (); iter ++)
    {
      iter->Serialize (i);
      i.Next(iter->GetSerializedSize ());
    }
}

uint32_t FatTreeIpv4RoutingHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  uint8_t temp;

  temp = i.ReadU8 ();
  NS_ASSERT_MSG (temp == 1, "FatTreeRouting received a message with mismatch version, aborting.");

  uint16_t temp16 = i.ReadU16 ();
  NS_ASSERT_MSG (temp16 == 0, "FatTreeRouting received a message with invalid filled flags, aborting.");

  uint8_t rteNumber = (i.GetSize () - 3)/6;

  for (uint8_t n=0; n < rteNumber; n++)
    {
      FatTreeIpv4Rte rte;
      i.Next (rte.Deserialize (i));
      m_rteList.push_back (rte);
    }

  return GetSerializedSize ();
}


void
FatTreeIpv4RoutingHeader::AddRte (FatTreeIpv4Rte rte)
{
  m_rteList.push_back (rte);
}

void
FatTreeIpv4RoutingHeader::ClearRtes ()
{
  m_rteList.clear ();
}

uint16_t
FatTreeIpv4RoutingHeader::GetRteNumber (void) const
{
  return m_rteList.size ();
}

std::list<FatTreeIpv4Rte>
FatTreeIpv4RoutingHeader::GetRteList (void) const
{
  return m_rteList;
}


std::ostream & operator << (std::ostream & os, const FatTreeIpv4RoutingHeader & h)
{
  h.Print (os);
  return os;
}

FatTreeIpv4RoutingHeader::~FatTreeIpv4RoutingHeader()
{
}

} /* namespace ns3 */
