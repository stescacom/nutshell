/* 
 * File:   ComputationalNode.h
 * Author: ubaid <u.ur.rahman@gmail.com>
 *
 * Created on July 3, 2015, 4:35 PM
 */

#ifndef COMPUTATIONALNODE_H
#define	COMPUTATIONALNODE_H

#include "processing-power-util.h"
#include "storage-util.h"
#include "ns3/node.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/data-rate.h"
#include "node-utilization.h"

namespace ns3 {
    
    /**
     * \brief A computational Node extending the network Node of simulator.
     *
     * This class holds together:
     *   - All parts of network node provided by Node class.
     *   - Storage: Amount of storage on the computational node.
     *   - Memory: Amount of memory on the node. 
     *   - Processing power: The amount of processing power available on the node
     *       in flops (floating point operations per second) or MIPS (Million of instruction per second).
     *   - Virtual Machines: Virtual machines currently running on the node with
     *       unique IDs. 
     *
     * Every Node created is added to the NodeList automatically.
     */

class ComputationalNode : public Node {
public:
    /**
    * \brief Get the type ID.
    * \return the object TypeId
    */
    static TypeId GetTypeId(void);
    /**
     * \brief ComputationalNode Constructor, the constructor allows creation of
     * computational node with default values
     */
    ComputationalNode();
    /**
     * \brief ComputationalNode Constructor, the constructor allows creation of
     * computational node with default values
    * \param systemId a unique integer used for parallel simulations.
    */
    ComputationalNode(uint32_t systemId);
    
    /**
     * \brief Constructor for creating a computational node with custom attribute values
     *
     * The constructor allows user to set the computation and storage properties
     * along with system ID.
     * 
     * \param systemId
     * \param processingPower the processing power of node
	 * \param primaryStorage the primary storage of node
	 * \param secondaryStorage the secondary storage of node
     */
    
    ComputationalNode(uint32_t systemId,
                        ProcessingPower processingPower,
                        Storage primaryStorage,
                        Storage secondaryStorage);

    /**
	 * \brief Constructor for creating a computational node with custom attribute values
	 *
	 * The constructor allows user to set the computation and storage properties.
	 *
	 * \param systemId the system ID
	 * \param processingPower the processing power of node
	 * \param primaryStorage the primary storage of node
	 * \param secondaryStorage the secondary storage of node
	 */
    ComputationalNode(ProcessingPower processingPower,
						Storage primaryStorage,
						Storage secondaryStorage);
    /**
     * \brief Modify existing resources values
     *
     * \param processingPower the processing power of node
	 * \param primaryStorage the primary storage of node
	 * \param secondaryStorage the secondary storage of node
     */
    void ModifyResources(ProcessingPower processingPower,
						Storage primaryStorage,
						Storage secondaryStorage);


    virtual ~ComputationalNode();
    
    /**
     * \brief Gets the primaryStorage object
     * \return primaryStorage of Storage type
     */
    
    Storage GetPrimaryStorage(void) const;
    
    /**
     * \brief Returns the processingPower object
     * \return processingPower of ProcessingPower type.
     */

    ProcessingPower GetProcessingPower(void) const;
    
    /**
     * \brief Returns the secondary storage of node, of Storage type
     * \return secondaryStorage Storage type
     */

    Storage GetSecondaryStorage(void) const ;
    
    /**
     * \brief Set the NIC data rate
     *
     * The NIC data rate is checked by VM scheduler, when assigning
     * VM to the node, the VM checks if the NIC can handle the data rate
     * requested by the VM
     *
     * \param rate DataRate object
     */
    void SetNicDataRate(DataRate rate);
    /**
     * \brief Gets the NIC data rate
     *	\retun the data rate of NIC
     */
    DataRate GetNicDataRate();


    /**
     * \brief The method allows to reserve or allocate resources to a virtual machine.
     *
     * The method is used to reserve resources for a VM. Requirements
     * of VM are passed to this function and the requested amount is allocated to the
     * VM if falls in the available amount of resources.
     *
     * \param processingPower the processing power of VM a ProcessingPower object
     * \param primaryStorage the primary storage of VM a Storage object
     * \param secondaryStorage the secondary storage of VM a Storage object
     * \return boolean true in case the reservation successful otherwise false.
     */
   
    bool ReserveResources(ProcessingPower processingPower, Storage primaryStorage,
    		Storage secondaryStorage);
    /**
     * \brief The method reserves NIC data rate for VM.
     *
     * The method is used to reserve the data rate for VM. The requested amount
     * of data rate is passed to this method, and checked if available, then resources are
     * allocated.
     *
     * \param rate a DataRate object
     * \return boolean true in case the resources are allocated, false otherwise
     */
    bool ReserveNicRate(DataRate rate);
    /**
     * \brief This method release resources consumed by a Virtual machine.
     *
     * Request to release resources is sent to computational node via this method. Resources details
     * of are provided as argument.
     *
     * \param processingPower the processing power of VM a ProcessingPower object
     * \param primaryStorage the primary storage of VM a Storage object
     * \param secondaryStorage the secondary storage of VM a Storage object.
     * \return boolean true if the release was successful, false otherwise.
     */
    bool ReleaseResources(ProcessingPower processingPower, Storage primaryStorage,
    		Storage secondaryStorage);

    /**
     * \brief This method release the data rate reserved by VM.
     *
     * Request to release NIC reserved data rate is sent to computational node via this method.
     * The reserved data rate is provided in argument.
     *
     * \param rate the data rate of VM, a DataRate object.
     * \return boolean true if the release was successful, false otherwise.
     */
    bool ReleaseNicRate(DataRate rate);


    bool CheckComputeResourceAvailability(ProcessingPower p, Storage ps, Storage ss);

    /**
     * \brief Checks the availability of resources
     *
     * This method is used to check the availability of requested amount of
     * resources.
     *
     * \param p the processing power of VM, a ProcessingPower object
     * \param ps the primary storage of VM, a Storage object
     * \param ss the secondary storage of VM, a Storage object.
     * \return boolean true if available, false otherwise.
     */
    bool CheckAvailability(ProcessingPower p, Storage ps, Storage ss,
    		DataRate r);
    

protected:
    /**
     * The dispose method. Subclasses must override this method
     * and must chain up to it by calling ComputationalNode::DoDispose at the
     * end of their own DoDispose method.
     */
    virtual void DoDispose(void);
    virtual void DoInitialize(void);
private:
    /**
     * \brief Initialize and aggregates utilization object to node
     *
     */
    void InitializeUtilization();


    /**
     * \brief Copy constructor for ComputationalNode object.
     *
     * It accepts ComputationalNode object and copies all of its attributes
     * to the new object.
     *
     * \param orig a ComputationalNode object
     */
    ComputationalNode(const ComputationalNode& orig);
    /**
     * \brief Checks the compatibility of processing power
     *
     * \param rhs a processing power of another source, a ProcessingPower object.
     * \return boolean true if compatible, false otherwise.
     */
    bool IsProcessingCompatible (ProcessingPower rhs);

    /**
     * \brief Checks if processing power is enough to facilitate arriving object
     * \param processingPower the processing power of incoming object, a ProcessingPower object
     * \return boolean true if available, false otherwise.
     */    
    bool IsProcessingPowerAvailable(ProcessingPower processingPower);
    
    /**
     * \brief Checks if storage is enough to facilitate arriving object
     * \param target the target storage of node to check availability.
     * \param requested the storage of incoming object, a Storage object.
     * \return boolean true if available, false otherwise.
     */
    bool IsStorageAvailable(Storage target, Storage requested );
    /**
     * \brief Check if available data rate of NIC is enough to facilitate arriving object.
     * \param r the data rate of incoming object, a DataRate object.
     * \return boolean true if available, false otherwise.
     */
    bool IsNicRateAvailable(DataRate r);
     
    ProcessingPower m_processingPower; //!< Computational node's processing power in flops or mips
    Storage m_secondaryStorage; //!< Secondary storage capacity of node.
    Storage m_primaryStorage; //!< Primary memory of node i.e. RAM
    DataRate m_nicDataRate; //!< NIC DataRate


    Ptr<NodeUtilization> m_utilization; //!< Node Utilization
    
};

}

#endif	/* COMPUTATIONALNODE_H */

