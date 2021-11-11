/*
 * virtual-machine.cc
 *
 *  Created on: Aug 19, 2015
 *      Author: ubaid
 */

#include "virtual-machine.h"

#include "ns3/log.h"
#include "ns3/address.h"
#include "ns3/inet-socket-address.h"
#include "ns3/inet6-socket-address.h"
#include "ns3/packet-socket-address.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/data-rate.h"
#include "ns3/random-variable-stream.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/tcp-socket-factory.h"
#include "ns3/string.h"
#include "ns3/pointer.h"
#include "ns3/boolean.h"

#include "computational-node.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("VirtualMachine");

NS_OBJECT_ENSURE_REGISTERED(VirtualMachine);

TypeId
VirtualMachine::GetTypeId()
{
	static TypeId tid =	TypeId("ns3::VirtualMachine")
				.SetParent<Application>()
				.AddConstructor<VirtualMachine>()
				.AddAttribute("ProcessingPower",
						"The processing power of  Machine",
						ProcessingPowerValue(ProcessingPower("1GFLOPS")),
						MakeProcessingPowerAccessor(&VirtualMachine::m_processingPower),
						MakeProcessingPowerChecker())
				.AddAttribute("PrimaryStorage",
						"The primary storage (RAM) of  Machine",
						StorageValue(Storage("8GB")),
						MakeStorageAccessor(&VirtualMachine::m_primaryStorage),
						MakeStorageChecker())
				.AddAttribute("SecondaryStorage",
						"The secondary storage of  Machine",
						StorageValue(Storage("500GB")),
						MakeStorageAccessor(&VirtualMachine::m_secondaryStorage),
						MakeStorageChecker())
				.AddAttribute("ApplicationSize",
						"The size of application in number of instruction/operations which is being executed on  machine",
						ApplicationSizeValue(ApplicationSize("10MFLOP")),
						MakeApplicationSizeAccessor(&VirtualMachine::m_applicationSize),
						MakeApplicationSizeChecker());
	return tid;
}

VirtualMachine::VirtualMachine()
	: m_processingPower("1GFLOPS"),
	  m_primaryStorage("8GB"),
	  m_secondaryStorage("500GB"),
	  m_applicationSize("10MFLOP")
{
	NS_LOG_FUNCTION(this);
	m_cnode = 0;
}

VirtualMachine::VirtualMachine(const ProcessingPower& power,
								const Storage& pStorage,
								const Storage& sStorage,
								const ApplicationSize& appSize)
	: m_processingPower(power),
	  m_primaryStorage(pStorage),
	  m_secondaryStorage(sStorage),
	  m_applicationSize(appSize)
{
	NS_LOG_FUNCTION(this);
}

VirtualMachine::~VirtualMachine()
{
	NS_LOG_FUNCTION(this);
}

/*
 * ----------------- setter and getters --------------
 */

void
VirtualMachine::SetProcessingPower(const ProcessingPower& power)
{
	m_processingPower = power;
}
void
VirtualMachine::SetPrimaryStorage(const Storage& pStorage)
{
	m_primaryStorage = pStorage;
}
void
VirtualMachine::SetSecondaryStorage(const Storage& sStorage)
{
	m_secondaryStorage = sStorage;
}
void
VirtualMachine::SetApplicationSize(const ApplicationSize& appSize)
{
	m_applicationSize = appSize;
}

void
VirtualMachine::SetCompNode(Ptr<ComputationalNode> node)
{
	m_cnode = node;
}

ProcessingPower
VirtualMachine::GetProcessingPower(void) const
{
	return m_processingPower;
}
Storage
VirtualMachine::GetPrimaryStorage(void) const
{
	return m_primaryStorage;
}
Storage
VirtualMachine::GetSecondaryStorage(void) const
{
	return m_secondaryStorage;
}
ApplicationSize
VirtualMachine::GetApplicationSize(void) const
{
	return m_applicationSize;
}

Ptr<ComputationalNode>
VirtualMachine::GetCompNode() const
{
	return m_cnode;
}

bool
VirtualMachine::IsResourcesReserved() const
{
	return m_reservedResources;
}

/*
 * ----------------- end of setter and getters section -----
 */

double
VirtualMachine::CalculateProcessingTime(void)
{
	double t = 0;
	if(m_applicationSize.IsFlop())
	{
		t = m_processingPower.CalculateExTime(m_applicationSize.GetApplicationSize(), ProcessingPower::METRIC_FLOP);
	}
	else if( m_applicationSize.IsInstruction())
	{
		t = m_processingPower.CalculateExTime(m_applicationSize.GetApplicationSize(), ProcessingPower::METRIC_INSTRUCTION_COUNT);
	}
	return t;
}

/*
 * ------------- protected methods -------------
 */

void
VirtualMachine::DoDispose()
{
	NS_LOG_FUNCTION(this);
//	Application::DoDispose();
}

void
VirtualMachine::ScheduleStop(double time)
{
	NS_LOG_FUNCTION(this);
	Simulator::Schedule(Seconds(time), &VirtualMachine::StopApplication, this);
}

bool
VirtualMachine::ReserveResources()
{
	NS_LOG_FUNCTION(this);

	if(m_cnode != 0)
	{
		if(m_cnode->ReserveResources(m_processingPower, m_primaryStorage, m_secondaryStorage))
		{
			m_reservedResources = true;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		NS_FATAL_ERROR("Unable to reserve resources, no computational node defined");
		return false;
	}

}
void
VirtualMachine::ReleaseResources()
{
	NS_LOG_FUNCTION(this);
	m_cnode->ReleaseResources(m_processingPower, m_primaryStorage, m_secondaryStorage);
}

/*
 * ------------ end of protected methods------------
 */

/*
 * -------------private methods--------
 */

void
VirtualMachine::StartApplication()
{
	NS_LOG_FUNCTION(this);
	if(ReserveResources())
	{
		double stopTime = CalculateProcessingTime();
		ScheduleStop(stopTime);
	}
	else
	{
		StopApplication();
	}

}

void
VirtualMachine::StopApplication()
{
	NS_LOG_FUNCTION(this);
	if(m_reservedResources)
	{
		ReleaseResources();
	}
}

/*
 * ------------end of private methods
 */

} /* namespace ns3 */
