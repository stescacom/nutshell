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
 * virtual-machine-helper.h
 *
 *  Created on: Oct 6, 2015
 *      Author: ubaid
 */

#ifndef VIRTUAL_MACHINE_HELPER_H
#define VIRTUAL_MACHINE_HELPER_H

#include <stdint.h>
#include <string>
#include "ns3/object-factory.h"
#include "ns3/attribute.h"
#include "ns3/node-container.h"

#include "processing-power-util.h"
#include "storage-util.h"
#include "application-size-util.h"

#include "computational-node.h"
#include "computational-node-container.h"
#include "vm-container.h"



namespace ns3 {

class VirtualMachineHelper {
public:
	enum VmType_e
	{
		COMPUTATIONAL_LOCAL_DATA,
		PRODUCER,
		CONSUMER_PRODUCER,
		CONSUMER
	};
	VirtualMachineHelper(VmType_e type);
	/**
	* Helper function used to set the underlying Virtual machine attributes.
	*
	* \param name the name of the Virtual machine attribute to set
	* \param value the value of the Virtual machine attribute to set
	*/
	void SetAttribute (std::string name, const AttributeValue &value);
	/**
	* Install a virtual machine of selected type on each node of the input container
	* configured with all the attributes set with SetAttribute.
	*
	* \param c NodeContainer of the set of nodes on which the selected VM
	* will be installed.
	* \returns Container of Ptr to the applications installed.
	*/
	VmContainer Install (ComputationalNodeContainer c) const;

	/**
	* Install a virtual machine of selected type on the node configured with all the
	* attributes set with SetAttribute.
	*
	* \param node The node on which the selected VM will be installed.
	* \returns Container of Ptr to the Virtual machine installed.
	*/
	VmContainer Install (Ptr<ComputationalNode> node) const;

	virtual ~VirtualMachineHelper();
private:
	/**
	* Install a virtual machine of selected type on the node configured with all the
	* attributes set with SetAttribute.
	*
	* \param node The node on which the selected VM will be installed.
	* \returns Ptr to the application installed.
	*/
	Ptr<VirtualMachine> InstallPriv (Ptr<ComputationalNode> node) const;

	ObjectFactory m_factory; //!< Object factory.
	VmType_e m_selectedTypeId;
};

} /* namespace ns3 */

#endif /* SCRATCH_NUTSHELL_VIRTUAL_MACHINE_HELPER_H_ */
