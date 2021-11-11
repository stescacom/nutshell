/*
 * File:   c-node-block.cc
 * Author: ubaid <u.ur.rahman@gmail.com>
 *
 * Created on July 3, 2015, 4:35 PM
 */


#include <vector>
#include "ns3/netanim-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/nstime.h"
#include "ns3/random-variable-stream.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/gtk-config-store.h"

#include "computational-local-data-vm.h"
#include "virtual-machine-helper.h"
#include "consumer-vm.h"
#include "consumer-producer-vm.h"
#include "producer-vm.h"
#include "network-vm.h"
#include "computational-node.h"
#include "computational-node-container.h"
#include "processing-power-util.h"
#include "storage-server.h"
#include "storage-util.h"
#include "connection-helper.h"
#include "addressing-scheme.h"
#include "fat-tree-addressing-scheme.h"
#include "access-network.h"
#include "three-tier-config.h"
#include "three-tier.h"

#include "fat-tree-config.h"
#include "fat-tree.h"
#include "fat-tree-ipv4-routing-header.h"
#include "fat-tree-ipv4-routing-protocol-helper.h"
#include "fat-tree-ipv4-routing-protocol.h"
#include "fat-tree-ipv4-rte.h"

#include "virtual-machine-helper.h"

#include "vm-scheduler.h"
#include "fcfs-first-fit-vm-scheduler.h"

#include "nutshell-data-collector.h"
#include "node-data.h"
#include "vm-data.h"
#include "queue-data.h"
#include "network-data.h"
#include "sjf-first-fit-vm-scheduler.h"



//#include "dataCollector/node-data.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("NutshellExperiment");


int
main ( int argc, char** argv )
{

//	Config::SetDefault ("ns3::Ipv4GlobalRouting::RandomEcmpRouting", BooleanValue (true));
    Time::SetResolution (Time::NS);

//    LogComponentEnable("VirtualMachine", LOG_LEVEL_INFO);
//    LogComponentEnable("ProducerVm", LOG_LEVEL_INFO);
//    LogComponentEnable("ConsumerProducerVm", LOG_LEVEL_INFO);
//    LogComponentEnable("ConsumerVm", LOG_LEVEL_INFO);
//    LogComponentEnable("ComputationalNode", LOG_LEVEL_INFO);
//    LogComponentEnable("NetworkVm", LOG_LEVEL_INFO);
//    LogComponentEnable("PacketSink", LOG_LEVEL_INFO);
//    LogComponentEnable("StorageServer", LOG_LEVEL_INFO);
//    LogComponentEnable("FcfsFirstFitVmScheduler", LOG_LEVEL_INFO);
//    LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
//    LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);
//    LogComponentEnable("FatTreeIpv4RoutingProtocol", LOG_LEVEL_ALL);
//    LogComponentEnable("FatTreeIpv4RoutingProtocol", LOG_LEVEL_FUNCTION);

    LogComponentEnable("Config", LOG_LEVEL_ALL);
//	LogComponentEnable("NodeUtilization", LOG_LEVEL_ALL);

//    ConfigStore config;
//    config.ConfigureDefaults ();

    NutshellDataCollector collector;
    collector.SetSimulationPrefix("FatTree");

//
//    NodeData nd;
//    nd.SetTimeInterval(6);
//    nd.EnableIndividualNodeData(true);
//
//	NetworkData netStat;
//	netStat.SetTimeInterval(6);
//
	VmData vmd;

    PointToPointHelper p2p;
    p2p.SetDeviceAttribute ("DataRate", StringValue ("1Gbps"));
    p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

    FatTreeConfig conf;
	conf.ConfigureNode("500TFLOPS", "1000TFLOPS", "10GB", "30GB", "1TB", "10TB");
	conf.ConfigureVmGeneral(700, "200TFLOPS", "800TFLOPS", "10GB", "20GB", "500GB", "1TB", "5000TFLOP", "10000TFLOP");
	conf.ConfigureVmDataReq(true, 2, DatacenterConfig::RANDOM,"50MB","1GB", "300MB", "300MB", "9600MB", "9600MB",
				50, 300, 70.0, 80.0, MilliSeconds(12), MicroSeconds(12), NanoSeconds(100), NanoSeconds(100));
	conf.ConfigureVmSplits(true, "3:2:1");
	conf.ConfigureVmArrivals(Seconds(2.0), Seconds(25.0));
	conf.ConfigureVmNetwork(1500, "ns3::TcpSocketFactory", "100Mbps");
	conf.ConfigureStorageServer(5);
    conf.SetBaseNetwork("10.0.0.0", "255.255.255.0");
	conf.SetLink(p2p);
	conf.SetPods(8);

	SjfFirstFitVmScheduler sch;
	conf.SetVmScheuler(sch);
	vmd.SetScheduler(sch);

//	collector.AddDataCollector(nd);
//	collector.AddDataCollector(netStat);
	collector.AddDataCollector(vmd);

	conf.EnableTracing(collector);

	FatTree fat(conf);


//    NutshellDataCollector collector2;
//	collector2.SetSimulationPrefix("ThreeTier");
//
//	NodeData nd;
//	nd.SetTimeInterval(6);
//  nd.EnableIndividualNodeData(true);
//
//	NetworkData netStat;
//	netStat.SetTimeInterval(6);
//
//	collector2.AddDataCollector(nd);
//	collector2.AddDataCollector(netStat);
//
//	PointToPointHelper nTacP2p, acTagP2p, agTcP2p;
//
//	nTacP2p.SetDeviceAttribute ("DataRate", StringValue ("1Gbps"));
//	nTacP2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
//
//	acTagP2p.SetDeviceAttribute ("DataRate", StringValue ("1Gbps"));
//	acTagP2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
//
//	agTcP2p.SetDeviceAttribute ("DataRate", StringValue ("1Gbps"));
//	agTcP2p.SetChannelAttribute ("Delay", StringValue ("2ms"));
//
//	ThreeTierConfig conf2;
//
//	conf2.ConfigureNode("50TFLOPS", "100TFLOPS", "10GB", "30GB", "1TB", "10TB");
//	conf2.ConfigureVmGeneral(7, "20TFLOPS", "40TFLOPS", "10GB", "20GB", "500GB", "1TB", "5000TFLOP", "10000TFLOP");
//	conf2.ConfigureVmDataReq(true, 2, DatacenterConfig::RANDOM,"1GB","10GB", "300MB", "300MB", "9600MB", "9600MB",
//				50, 300, 70.0, 80.0, MilliSeconds(12), MicroSeconds(12), NanoSeconds(100), NanoSeconds(100));
//	conf2.ConfigureVmSplits(true, "3:2:1");
//	conf2.ConfigureVmArrivals(Seconds(2.0), Seconds(25.0));
//	conf2.ConfigureVmNetwork(1500, "ns3::TcpSocketFactory", "100Mbps");
//	conf2.ConfigureStorageServer(5);
//
//	// topology
//	conf2.SetNodesPerAccessSwitch(2);
//	conf2.SetSwitchesNumber(12,8,4);
//	conf2.SetPods(4);
//	conf2.SetNetwork("10.0.0.0", "255.255.255.0");
//	conf2.SetLinks(nTacP2p, acTagP2p, agTcP2p);
//
//
//	conf.EnableTracing(collector2);
//
//	ThreeTier tt(conf2);

//	config.ConfigureAttributes ();

	Simulator::Stop(Seconds(10));
    Simulator::Run ();
    Simulator::Destroy ();
    collector.Export();
//    collector2.Export();


    return 0;
}
