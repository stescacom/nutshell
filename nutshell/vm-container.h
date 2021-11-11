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
 * vm-container.h
 *
 *  Created on: Dec 28, 2015
 *      Author: ubaid
 *       Email: u.ur.rahman@gmail.com
 */

#ifndef NUTSHELL_VM_CONTAINER_H
#define NUTSHELL_VM_CONTAINER_H

#include <stdint.h>
#include <vector>
#include "ns3/application.h"

#include "virtual-machine.h"

namespace ns3 {

class VmContainer {
public:
	VmContainer();
	 /**
	   * Create an ApplicationContainer with exactly one application which has
	   * been previously instantiated.  The single application is specified
	   * by a smart pointer.
	   *
	   * \param application The Ptr<VirtualMachine> to add to the container.
	   */
	VmContainer (Ptr<VirtualMachine> vm);

	  /// Application container iterator
	  typedef std::vector<Ptr<VirtualMachine> >::const_iterator Iterator;

	  Iterator Begin (void) const;
	  Iterator End (void) const;
	  uint32_t GetN (void) const;
	  Ptr<VirtualMachine> Get (uint32_t i) const;
	  void Add (VmContainer other);
	  void Add (Ptr<VirtualMachine> vm);
	  void Start (Time start);
	  void Stop (Time stop);
	  virtual ~VmContainer();
	private:
	  std::vector<Ptr<VirtualMachine> > m_vms;

};

} /* namespace ns3 */

#endif /* SCRATCH_NUTSHELL_VM_CONTAINER_H_ */
