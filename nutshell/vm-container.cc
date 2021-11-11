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
 * vm-container.cc
 *
 *  Created on: Dec 28, 2015
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#include <stdint.h>
#include <vector>
#include "ns3/application.h"

#include "virtual-machine.h"

#include "vm-container.h"

namespace ns3 {

VmContainer::VmContainer() {
	// TODO Auto-generated constructor stub

}

VmContainer::VmContainer (Ptr<VirtualMachine> app)
{
  m_vms.push_back (app);
}


VmContainer::Iterator
VmContainer::Begin (void) const
{
  return m_vms.begin ();
}
VmContainer::Iterator
VmContainer::End (void) const
{
  return m_vms.end ();
}

uint32_t
VmContainer::GetN (void) const
{
  return m_vms.size ();
}
Ptr<VirtualMachine>
VmContainer::Get (uint32_t i) const
{
  return m_vms[i];
}

void
VmContainer::Add (VmContainer other)
{
  for (Iterator i = other.Begin (); i != other.End (); i++)
    {
      m_vms.push_back (*i);
    }
}
void
VmContainer::Add (Ptr<VirtualMachine> vm)
{
  m_vms.push_back (vm);
}

void
VmContainer::Start (Time start)
{
  for (Iterator i = Begin (); i != End (); ++i)
    {
      Ptr<VirtualMachine> app = *i;
      app->SetStartTime (start);
    }
}
void
VmContainer::Stop (Time stop)
{
  for (Iterator i = Begin (); i != End (); ++i)
    {
      Ptr<VirtualMachine> app = *i;
      app->SetStopTime (stop);
    }
}

VmContainer::~VmContainer() {
	// TODO Auto-generated destructor stub
}

} /* namespace ns3 */
