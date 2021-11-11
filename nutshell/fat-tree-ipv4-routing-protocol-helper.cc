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
 * fat-tree-ipv4-routing-protocol-helper.cc
 *
 *  Created on: Nov 25, 2015
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#include "ns3/log.h"

#include "fat-tree-ipv4-routing-protocol.h"

#include "fat-tree-ipv4-routing-protocol-helper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("FatTreeIpv4RoutingProtocolHelper");

FatTreeIpv4RoutingProtocolHelper::FatTreeIpv4RoutingProtocolHelper()
{
}

FatTreeIpv4RoutingProtocolHelper::FatTreeIpv4RoutingProtocolHelper(const FatTreeIpv4RoutingProtocolHelper &)
{

}
FatTreeIpv4RoutingProtocolHelper*
FatTreeIpv4RoutingProtocolHelper::Copy (void) const
{
	return new FatTreeIpv4RoutingProtocolHelper (*this);
}

Ptr<Ipv4RoutingProtocol>
FatTreeIpv4RoutingProtocolHelper::Create (Ptr<Node> node) const
{
	// creating FatTreeIpv4RoutingProtocol object and aggrigating it to node
	// and returning the object
	Ptr<FatTreeIpv4RoutingProtocol> agent = CreateObject<FatTreeIpv4RoutingProtocol>();
	node->AggregateObject(agent);
	return agent;
}

FatTreeIpv4RoutingProtocolHelper::~FatTreeIpv4RoutingProtocolHelper()
{
}

} /* namespace ns3 */
