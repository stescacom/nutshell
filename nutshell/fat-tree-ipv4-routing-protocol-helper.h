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
 * fat-tree-ipv4-routing-protocol-helper.h
 *
 *  Created on: Nov 25, 2015
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#ifndef FAT_TREE_IPV4_ROUTING_PROTOCOL_HELPER_H
#define FAT_TREE_IPV4_ROUTING_PROTOCOL_HELPER_H

#include "ns3/node-container.h"
#include "ns3/ipv4-routing-helper.h"

namespace ns3 {

class FatTreeIpv4RoutingProtocolHelper: public Ipv4RoutingHelper {
public:
	FatTreeIpv4RoutingProtocolHelper();
	FatTreeIpv4RoutingProtocolHelper(const FatTreeIpv4RoutingProtocolHelper &);
	FatTreeIpv4RoutingProtocolHelper* Copy (void) const;
	virtual Ptr<Ipv4RoutingProtocol> Create (Ptr<Node> node) const;
	virtual ~FatTreeIpv4RoutingProtocolHelper();

};



} /* namespace ns3 */

#endif /* FAT_TREE_IPV4_ROUTING_PROTOCOL_HELPER_H */
