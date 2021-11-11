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
 * computational-node-container.cc
 *
 *  Created on: Sep 15, 2015
 *      Author: ubaid
 */

#include <stdlib.h>

#include "computational-node-container.h"
#include "ns3/node-list.h"
#include "ns3/names.h"
#include "ns3/node-container.h"
#include "ns3/log.h"
#include "ns3/random-variable-stream.h"
#include "processing-power-util.h"
#include "storage-util.h"

namespace ns3 {

ComputationalNodeContainer::ComputationalNodeContainer()
{
}

ComputationalNodeContainer::~ComputationalNodeContainer()
{
}

ComputationalNodeContainer::ComputationalNodeContainer (Ptr<ComputationalNode> node)
{
  m_nodes.push_back (node);
}
ComputationalNodeContainer::ComputationalNodeContainer (std::string nodeName)
{
  Ptr<ComputationalNode> node = Names::Find<ComputationalNode> (nodeName);
  m_nodes.push_back (node);
}
ComputationalNodeContainer::ComputationalNodeContainer (const ComputationalNodeContainer &a, const ComputationalNodeContainer &b)
{
  Add (a);
  Add (b);
}
ComputationalNodeContainer::ComputationalNodeContainer (const ComputationalNodeContainer &a, const ComputationalNodeContainer &b,
                              const ComputationalNodeContainer &c)
{
  Add (a);
  Add (b);
  Add (c);
}
ComputationalNodeContainer::ComputationalNodeContainer (const ComputationalNodeContainer &a, const ComputationalNodeContainer &b,
                              const ComputationalNodeContainer &c, const ComputationalNodeContainer &d)
{
  Add (a);
  Add (b);
  Add (c);
  Add (d);
}

ComputationalNodeContainer::ComputationalNodeContainer (const ComputationalNodeContainer &a, const ComputationalNodeContainer &b,
                              const ComputationalNodeContainer &c, const ComputationalNodeContainer &d,
                              const ComputationalNodeContainer &e)
{
  Add (a);
  Add (b);
  Add (c);
  Add (d);
  Add (e);
}

ComputationalNodeContainer::Iterator
ComputationalNodeContainer::Begin (void) const
{
  return m_nodes.begin ();
}
ComputationalNodeContainer::Iterator
ComputationalNodeContainer::End (void) const
{
  return m_nodes.end ();
}

uint32_t
ComputationalNodeContainer::GetN (void) const
{
  return m_nodes.size ();
}
Ptr<ComputationalNode>
ComputationalNodeContainer::Get (uint32_t i) const
{
  return m_nodes[i];
}
NodeContainer
ComputationalNodeContainer::GetNodeContainerOfRange(uint32_t start, uint32_t end)
{
	NodeContainer nodes;
	for (uint32_t i = start; i < end; i++)
	{
//		NS_LOG_UNCOND("First address: " << m_nodes[i]);
		nodes.Add(m_nodes[i]);
	}
	return nodes;
}

void
ComputationalNodeContainer::SetConfigRangeStrings(const std::string minPower,
													const std::string maxPower,
													const std::string minPrimary,
													const std::string maxPrimary,
													const std::string minSecondary,
													const std::string maxSecondary)
{
	m_config.processingMin = minPower;
	m_config.processingMax = maxPower;

	m_config.primaryMin = minPrimary;
	m_config.primaryMax = maxPrimary;

	m_config.secondaryMin = minSecondary;
	m_config.secondaryMax = maxSecondary;
}

void
ComputationalNodeContainer::Create (uint32_t n)
{
  for (uint32_t i = 0; i < n; i++)
    {
	  Ptr<ComputationalNode> newNode = CreateObject<ComputationalNode> ();
	  newNode->ModifyResources(GetProcessingValue(m_config.processingMin, m_config.processingMax),
			  GetStorageValue(m_config.primaryMin, m_config.primaryMax),
			  GetStorageValue(m_config.secondaryMin, m_config.secondaryMax));

      m_nodes.push_back (newNode);
    }
}
void
ComputationalNodeContainer::Create (uint32_t n, uint32_t systemId)
{
  for (uint32_t i = 0; i < n; i++)
    {
	  Ptr<ComputationalNode> newNode = CreateObject<ComputationalNode> (systemId);
	  newNode->ModifyResources(GetProcessingValue(m_config.processingMin, m_config.processingMax),
	  			  GetStorageValue(m_config.primaryMin, m_config.primaryMax),
	  			  GetStorageValue(m_config.secondaryMin, m_config.secondaryMax));

      m_nodes.push_back (newNode);
    }
}
void
ComputationalNodeContainer::Add (ComputationalNodeContainer other)
{
  for (Iterator i = other.Begin (); i != other.End (); i++)
    {
      m_nodes.push_back (*i);
    }
}
void
ComputationalNodeContainer::Add (Ptr<ComputationalNode> node)
{
  m_nodes.push_back (node);
}
void
ComputationalNodeContainer::Add (std::string nodeName)
{
  Ptr<ComputationalNode> node = Names::Find<ComputationalNode> (nodeName);
  m_nodes.push_back (node);
}

Storage
ComputationalNodeContainer::GetStorageValue(Storage min, Storage max)
{
	if(min == max)
	{
		return min;
	}
	else
	{
		Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable>();
		uint64_t newVal = uv->GetValue(min.GetStorage(), max.GetStorage());
		return Storage(newVal);
	}
}

ProcessingPower
ComputationalNodeContainer::GetProcessingValue (ProcessingPower min, ProcessingPower max)
{

	if(min == max)
	{
		return min;
	}
	else
	{
		Ptr<UniformRandomVariable> uv = CreateObject<UniformRandomVariable>();
		uint64_t newVal = uv->GetValue(min.GetProcessingPower(), max.GetProcessingPower());
		int metric;
		if(min.IsFlops() && max.IsFlops())
		{
			metric = ProcessingPower::POWER_FLOPS;
		}
		else if (min.IsMips() && max.IsMips())
		{
			metric = ProcessingPower::POWER_MIPS;
		}
		else
		{
			NS_FATAL_ERROR("Processing Powers do not match " << this);
		}

		return ProcessingPower(newVal, metric);
	}
}

} /* namespace ns3 */
