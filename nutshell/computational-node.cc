/* 
 * File:   ComputationalNode.h
 * Author: ubaid <u.ur.rahman@gmail.com>
 *
 * Created on July 3, 2015, 4:35 PM
 */

#include "computational-node.h"

#include "processing-power-util.h"
#include "storage-util.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"
#include "node-utilization.h"

namespace ns3 {
    
    NS_LOG_COMPONENT_DEFINE ("ComputationalNode");

    NS_OBJECT_ENSURE_REGISTERED (ComputationalNode);
    
    TypeId ComputationalNode::GetTypeId ()
    {
        static TypeId tid = TypeId("ns3::ComputationalNode")
            .SetParent <Node>()
            .AddConstructor<ComputationalNode> ()
            .AddAttribute ("ProcessingPower",
                            "The default processing power for a computational node",
                            ProcessingPowerValue(ProcessingPower("100TFLOPS")),
                            MakeProcessingPowerAccessor (&ComputationalNode::m_processingPower),
                            MakeProcessingPowerChecker())
            .AddAttribute ("PrimaryStorage",
                            "The default primary storage for a computational node",
                            StorageValue(Storage("32GB")),
                            MakeStorageAccessor(&ComputationalNode::m_primaryStorage),
                            MakeStorageChecker())
            .AddAttribute ("SecondaryStorage",
                            "The default secondary storage for a computational node",
                            StorageValue(Storage("2TB")),
                            MakeStorageAccessor(&ComputationalNode::m_secondaryStorage),
                            MakeStorageChecker())
        ;
        return tid;
            
    }

    void
	ComputationalNode::InitializeUtilization()
    {

    	m_utilization = CreateObject<NodeUtilization>();
		this->AggregateObject(m_utilization);
		m_utilization->SetNodeId(this->GetId());
		m_utilization->SetBaseResources(m_processingPower, m_primaryStorage, m_secondaryStorage);
    }

    ComputationalNode::ComputationalNode ( )
        : Node()
    {
        NS_LOG_FUNCTION(this);
        InitializeUtilization();
    }
    ComputationalNode::ComputationalNode ( uint32_t systemId )
        : Node (systemId)
    {
        NS_LOG_FUNCTION(this);
        InitializeUtilization();
    }
    ComputationalNode::ComputationalNode ( uint32_t systemId, ProcessingPower processingPower, Storage primaryStorage, Storage secondaryStorage )
        : Node (systemId)
    {
        NS_LOG_FUNCTION(this);
         m_processingPower = processingPower;
         m_primaryStorage = primaryStorage;
         m_secondaryStorage = secondaryStorage;
         InitializeUtilization();
    }
    
    ComputationalNode::ComputationalNode(ProcessingPower processingPower, Storage primaryStorage, Storage secondaryStorage)
    	: Node()
    {
    	NS_LOG_FUNCTION (this);
		m_processingPower = processingPower;
		m_primaryStorage = primaryStorage;
		m_secondaryStorage = secondaryStorage;
		InitializeUtilization();
    }

    ComputationalNode::ComputationalNode (const ComputationalNode& orig) 
    {
        NS_LOG_FUNCTION(this);
        InitializeUtilization();
    }

    void
	ComputationalNode::ModifyResources(ProcessingPower processingPower,
    						Storage primaryStorage,
    						Storage secondaryStorage)
    {
    	NS_LOG_FUNCTION(this);
//    	NS_LOG_UNCOND("Node New: " << processingPower <<" | " << primaryStorage << " | " << secondaryStorage);
    	m_processingPower = processingPower;
		m_primaryStorage = primaryStorage;
		m_secondaryStorage = secondaryStorage;

		m_utilization->SetBaseResources(m_processingPower, m_primaryStorage, m_secondaryStorage);

    }

    ComputationalNode::~ComputationalNode ( )
    {
        NS_LOG_FUNCTION(this);
    }
    
    Storage
    ComputationalNode::GetPrimaryStorage ( void ) const
    {
        return m_primaryStorage;
    }

    ProcessingPower
    ComputationalNode::GetProcessingPower ( void ) const
    {
        return m_processingPower;
    }
    Storage
    ComputationalNode::GetSecondaryStorage ( ) const
    {
        return m_secondaryStorage;
    }

    void
	ComputationalNode::SetNicDataRate(DataRate rate)
    {
    	m_nicDataRate = rate;
    }
    DataRate
	ComputationalNode::GetNicDataRate()
    {
    	return m_nicDataRate;
    }


    bool
    ComputationalNode::ReserveResources ( ProcessingPower processingPower, Storage primaryStorage, Storage secondaryStorage )
    {
        NS_LOG_FUNCTION(this);

        if(CheckComputeResourceAvailability(processingPower, primaryStorage, secondaryStorage))
        {
        	m_processingPower = m_processingPower - processingPower;
        	m_primaryStorage = m_primaryStorage - primaryStorage;
        	m_secondaryStorage = m_secondaryStorage - secondaryStorage;

        	m_utilization->ReserveResources(processingPower, primaryStorage, secondaryStorage);

        	return true;
        }
        else
        {


        	return false;
        }
    }

    bool
    ComputationalNode::ReleaseResources ( ProcessingPower processingPower, Storage primaryStorage, Storage secondaryStorage )
    {
        NS_LOG_FUNCTION (this);
        m_processingPower = m_processingPower + processingPower;
		m_primaryStorage = m_primaryStorage + primaryStorage;
		m_secondaryStorage = m_secondaryStorage + secondaryStorage;


		m_utilization->ReleaseResources(processingPower, primaryStorage, secondaryStorage);
		return true;
    }

    bool
	ComputationalNode::CheckComputeResourceAvailability(ProcessingPower p, Storage ps, Storage ss)
    {
    	if(IsProcessingPowerAvailable(p) &&
    			IsStorageAvailable(m_primaryStorage, ps) &&
				IsStorageAvailable(m_secondaryStorage, ss))
		{
    		return true;
    	}
    	else
    	{
    		return false;
    	}
    }

    bool
	ComputationalNode::CheckAvailability(ProcessingPower p, Storage ps, Storage ss, DataRate r)
    {

    	if(IsProcessingPowerAvailable(p) &&
    			IsStorageAvailable(m_primaryStorage, ps) &&
				IsStorageAvailable(m_secondaryStorage, ss) && IsNicRateAvailable(r))
		{
			return true;
		}
		else
		{
			return false;
		}
    }
    
    /*
        ----------------
     *  Protected methods
     * ------------------
     */


    void
    ComputationalNode::DoDispose ( )
    {
        NS_LOG_FUNCTION(this);
        Node::DoDispose ();
    }

    void
    ComputationalNode::DoInitialize ( )
    {
        NS_LOG_FUNCTION(this);
        Node::DoInitialize ();
    }
    
    
    /*
        ----------------
     *  private methods
     * ------------------
     */
    
    bool
    ComputationalNode::IsProcessingCompatible (ProcessingPower rhs)
    {
    	bool res = false;
    	if((m_processingPower.IsFlops () && rhs.IsFlops ()) || (m_processingPower.IsMips () && rhs.IsMips ()))
		{
    		res = true;
		}
//    	NS_LOG_UNCOND("PP COMPATIBLE: " << res);
        return res;
    }
    
    bool
    ComputationalNode::IsStorageAvailable (Storage target, Storage requested )
    {
    	bool res = false;
    	if(requested < target)
    	{
    		res = true;
    	}
//    	NS_LOG_UNCOND("Storage Available: " << res);
        return res;
    }
    bool ComputationalNode::IsNicRateAvailable(DataRate r)
    {
    	bool res = false;
    	if(r < m_nicDataRate)
    	{
    		res = true;
    	}
//    	NS_LOG_UNCOND("NIC Available: " << res);
    	return res;
    }
    bool
    ComputationalNode::IsProcessingPowerAvailable ( ProcessingPower processingPower )
    {
    	bool res = false;
        if(IsProcessingCompatible (processingPower))
        {
        	if(processingPower < m_processingPower)
        	{
        		res = true;
        	}
        }
//        NS_LOG_UNCOND("PP Available: " << res);
        return res;
    }
    

    bool
	ComputationalNode::ReserveNicRate(DataRate rate)
    {
    	m_nicDataRate = DataRate((m_nicDataRate.GetBitRate() - rate.GetBitRate()));
    	return true;
    }
    bool
	ComputationalNode::ReleaseNicRate(DataRate rate)
    {
    	m_nicDataRate = DataRate((m_nicDataRate.GetBitRate() + rate.GetBitRate()));
    	return true;
    }


}
