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
 * virtual-machine-helper.cc
 *
 *  Created on: Oct 6, 2015
 *      Author: ubaid
 */

#include "ns3/uinteger.h"
#include "ns3/names.h"
#include "ns3/string.h"
#include "ns3/log.h"

#include "vm-container.h"
#include "virtual-machine-helper.h"

#include "processing-power-util.h"
#include "storage-util.h"
#include "application-size-util.h"
#include "virtual-machine.h"
#include "computational-local-data-vm.h"
#include "network-vm.h"
#include "consumer-producer-vm.h"
#include "consumer-vm.h"
#include "producer-vm.h"


namespace ns3 {

VirtualMachineHelper::VirtualMachineHelper(VmType_e type)
{
	std::string tids[] = {"ns3::ComputationalLocalDataVm",
										"ns3::ProducerVm",
										"ns3::ConsumerProducerVm", "ns3::ConsumerVm"};
	std::string typeId = tids[type];
	m_selectedTypeId = type;
	m_factory.SetTypeId (typeId);
}

void
VirtualMachineHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

VmContainer
VirtualMachineHelper::Install (Ptr<ComputationalNode> node) const
{
  return VmContainer (InstallPriv (node));
}

VmContainer
VirtualMachineHelper::Install (ComputationalNodeContainer c) const
{
	VmContainer apps;
  for (ComputationalNodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<VirtualMachine>
VirtualMachineHelper::InstallPriv (Ptr<ComputationalNode> node) const
{
  Ptr<VirtualMachine> app;
  if(m_selectedTypeId == VirtualMachineHelper::COMPUTATIONAL_LOCAL_DATA)
  {
	  app = m_factory.Create<ComputationalLocalDataVm> ();
  }
  else if(m_selectedTypeId == VirtualMachineHelper::PRODUCER)
  {
	  app = m_factory.Create<ProducerVm> ();
  }
  else if(m_selectedTypeId == VirtualMachineHelper::CONSUMER_PRODUCER)
  {
	  app = m_factory.Create<ConsumerProducerVm> ();
  }
  else if(m_selectedTypeId == VirtualMachineHelper::CONSUMER)
  {
	  app = m_factory.Create<ConsumerVm> ();
  }
  else
  {
	  app = m_factory.Create<VirtualMachine> ();
  }
  app->SetCompNode(node);
  node->AddApplication (app);
  return app;
}
VirtualMachineHelper::~VirtualMachineHelper()
{
}

} /* namespace ns3 */
