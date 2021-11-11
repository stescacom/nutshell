/*
 * virtual-machine.h
 *
 *  Created on: Aug 19, 2015
 *      Author: ubaid
 */

#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H


#include "ns3/application.h"
#include "ns3/address.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/data-rate.h"
#include "ns3/traced-callback.h"

#include "application-size-util.h"
#include "processing-power-util.h"
#include "storage-util.h"
#include "computational-node.h"

namespace ns3 {


class VirtualMachine: public Application {

public:
	static TypeId GetTypeId();

	VirtualMachine();

	VirtualMachine(const ProcessingPower& power,
					const Storage& pStorage,
					const Storage& sStorage,
					const ApplicationSize& appSize);

	virtual ~VirtualMachine();

	void SetProcessingPower(const ProcessingPower& power);
	void SetPrimaryStorage(const Storage& pStorage);
	void SetSecondaryStorage(const Storage& sStorage);
	void SetApplicationSize(const ApplicationSize& appSize);
	void SetCompNode(Ptr<ComputationalNode> node );

	ProcessingPower GetProcessingPower(void) const;
	Storage GetPrimaryStorage(void) const;
	Storage GetSecondaryStorage(void) const;
	ApplicationSize GetApplicationSize(void) const;
	Ptr<ComputationalNode> GetCompNode() const;
	bool IsResourcesReserved() const;

	double CalculateProcessingTime();

protected:

	virtual void DoDispose (void);
	virtual void ScheduleStop(double time);
	bool ReserveResources();
	void ReleaseResources();

	bool					m_reservedResources;
private:
	ProcessingPower 		m_processingPower;
	Storage					m_primaryStorage;
	Storage					m_secondaryStorage;
	ApplicationSize			m_applicationSize;
	Ptr<ComputationalNode>	m_cnode;



	virtual void StartApplication (void);    // Called at time specified by Start
	virtual void StopApplication (void);     // Called at time specified by Stop
};

} /* namespace ns3 */

#endif /* SCRATCH_C_NODE_BLOCK_VIRTUAL_MACHINE_H_ */
