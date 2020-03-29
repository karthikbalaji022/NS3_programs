// ****************************************************************************************
//Self Organizing Networking Group (SONG) @ Kongu Engineering College (KEC) 
//kecsong2013@gmail.com		 kecsong@gmail.com
//*****************************************************************************************
// Program V : Creating TCP ON-OFF Application for VANET scenario (Mobility Model Included)

#include "ns3/ocb-wifi-mac.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wave-mac-helper.h"
#include "ns3/aodv-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/ns2-mobility-helper.h"
#include "ns3/point-to-point-module.h"
#include "ns3/wifi-module.h" 
//#include "ns3/tcp-congestion-ops.h" 
//#include "ns3/netanim-module.h"
#include "ns3/yans-wifi-helper.h"
#include "ns3/internet-stack-helper.h"
#include "ns3/ipv4-address-helper.h"
#include "ns3/ipv4-interface-container.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/config-store-module.h"
#include "ns3/internet-module.h"
#include "ns3/tcp-header.h"
#include "ns3/flow-monitor-helper.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("v2vc25_4");

int 
main (int argc, char *argv[])
{
  bool verbose = true;
 // bool tracing = true;
  bool enableFlowMonitor = true;
  std::string phyMode ("OfdmRate6MbpsBW10MHz");
  std::string traceFile = "scratch/v2v25.txt";
  double interval = 0.5; // seconds
 // uint32_t packetSize = 1000; // bytes

  CommandLine cmd;

  cmd.AddValue ("phyMode", "Wifi Phy mode", phyMode);
  cmd.AddValue ("EnableMonitor", "Enable Flow Monitor", enableFlowMonitor);
  cmd.AddValue ("interval", "interval (seconds) between packets", interval);
  cmd.AddValue ("verbose", "turn on all WifiNetDevice log components", verbose);
  //cmd.AddValue ("tracing", "Enable pcap tracing", tracing);
  cmd.Parse (argc,argv);
  //Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpNewReno"));
  Config::SetDefault ("ns3::TcpL4Protocol::SocketType", StringValue ("ns3::TcpCubic"));
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (1000));
  //Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold",
                    //  StringValue ("2200"));
 // Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold",
                  //    StringValue ("2200"));
 //Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", 
                    //  StringValue (phyMode));


  if (verbose)
    {
      LogComponentEnable ("TcpCubic", LOG_LEVEL_INFO);
      LogComponentEnable ("TcpCubic", LOG_LEVEL_INFO);
    }
  

  Ns2MobilityHelper ns2mobility = Ns2MobilityHelper (traceFile);
  NodeContainer vanetnodes;
  vanetnodes.Create (100);
  ns2mobility.Install ();


  // The below set of helpers will help us to put together the wifi NICs we want
  YansWifiPhyHelper wifiPhy =  YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  Ptr<YansWifiChannel> channel = wifiChannel.Create ();
  wifiPhy.SetChannel (channel);
  // ns-3 supports generate a pcap trace
  wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);
  NqosWaveMacHelper wifi80211pMac = NqosWaveMacHelper::Default ();
  Wifi80211pHelper wifi80211p = Wifi80211pHelper::Default ();
   wifi80211p.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                      "DataMode",StringValue (phyMode),
                                      "ControlMode",StringValue (phyMode));
  NetDeviceContainer vanetdevices = wifi80211p.Install (wifiPhy, wifi80211pMac, vanetnodes);


  //if (verbose)
    //{
    // wifi80211p.EnableLogComponents ();      // Turn on all Wifi 802.11p logging
   // }

  // Enable AODV
  AodvHelper aodv;
  InternetStackHelper stack;
  stack.SetRoutingHelper (aodv);
  stack.Install (vanetnodes);
     

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer interfaces = address.Assign (vanetdevices);

 // Choosing Source and Destination Node for tcp traffic Flow 

  NS_LOG_INFO ("Create Applications.");
  //uint16_t port = 9;   
 
// 8 traffic

  Ptr<Node> appSource1 = NodeList::GetNode (46);
  Ptr<Node> appSink1 = NodeList::GetNode (60);
  Ipv4Address remoteAddr1 = appSink1->GetObject<Ipv4> ()->GetAddress (1,0).GetLocal ();



  OnOffHelper onoff1 ("ns3::TcpSocketFactory", Address (InetSocketAddress (remoteAddr1, 1)));
  onoff1.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
  onoff1.SetAttribute ("PacketSize", UintegerValue (1000));
  ApplicationContainer apps1 = onoff1.Install (vanetnodes.Get (46));
  apps1.Start (Seconds (1));
  apps1.Stop (Seconds (1000));
  PacketSinkHelper sink1 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 1));
  apps1 = sink1.Install (vanetnodes.Get (60));
 // apps1.Start (Seconds (1));
//2

  Ptr<Node> appSource2 = NodeList::GetNode (66);
  Ptr<Node> appSink2 = NodeList::GetNode (2);
  Ipv4Address remoteAddr2 = appSink2->GetObject<Ipv4> ()->GetAddress (1,0).GetLocal ();



  OnOffHelper onoff2 ("ns3::TcpSocketFactory", Address (InetSocketAddress (remoteAddr2, 2)));
  onoff2.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
  onoff2.SetAttribute ("PacketSize", UintegerValue (1000));
  ApplicationContainer apps2 = onoff2.Install (vanetnodes.Get (66));
  apps2.Start (Seconds (1));
  apps2.Stop (Seconds (1000));
  PacketSinkHelper sink2 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 2));
  apps2 = sink2.Install (vanetnodes.Get (2));
  //apps2.Start (Seconds (1));

//3

  Ptr<Node> appSource3 = NodeList::GetNode (47);
  Ptr<Node> appSink3 = NodeList::GetNode (56);
  Ipv4Address remoteAddr3 = appSink3->GetObject<Ipv4> ()->GetAddress (1,0).GetLocal ();



  OnOffHelper onoff3 ("ns3::TcpSocketFactory", Address (InetSocketAddress (remoteAddr3, 3)));
  onoff3.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
  onoff3.SetAttribute ("PacketSize", UintegerValue (1000));
  ApplicationContainer apps3 = onoff3.Install (vanetnodes.Get (47));
  apps3.Start (Seconds (1));
  apps3.Stop (Seconds (1000));
  PacketSinkHelper sink3 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 3));
  apps3 = sink3.Install (vanetnodes.Get (56));
  //apps3.Start (Seconds (1));

//4

  Ptr<Node> appSource4 = NodeList::GetNode (40);
  Ptr<Node> appSink4 = NodeList::GetNode (14);
  Ipv4Address remoteAddr4 = appSink4->GetObject<Ipv4> ()->GetAddress (1,0).GetLocal ();



  OnOffHelper onoff4 ("ns3::TcpSocketFactory", Address (InetSocketAddress (remoteAddr4, 4)));
  onoff4.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
  onoff4.SetAttribute ("PacketSize", UintegerValue (1000));
  ApplicationContainer apps4 = onoff4.Install (vanetnodes.Get (40));
  apps4.Start (Seconds (1));
  apps4.Stop (Seconds (1000));
  PacketSinkHelper sink4 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 4));
  apps4= sink4.Install (vanetnodes.Get (14));
  //apps4.Start (Seconds (1));

//5

  Ptr<Node> appSource5 = NodeList::GetNode (88);
  Ptr<Node> appSink5 = NodeList::GetNode (32);
  Ipv4Address remoteAddr5 = appSink5->GetObject<Ipv4> ()->GetAddress (1,0).GetLocal ();



  OnOffHelper onoff5 ("ns3::TcpSocketFactory", Address (InetSocketAddress (remoteAddr5, 5)));
  onoff5.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
  onoff5.SetAttribute ("PacketSize", UintegerValue (1000));
  ApplicationContainer apps5 = onoff5.Install (vanetnodes.Get (88));
  apps5.Start (Seconds (1));
  apps5.Stop (Seconds (1000));
  PacketSinkHelper sink5 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 5));
  apps5= sink5.Install (vanetnodes.Get (32));
 // apps5.Start (Seconds (1));

//6
  Ptr<Node> appSource6 = NodeList::GetNode (70);
  Ptr<Node> appSink6 = NodeList::GetNode (18);
  Ipv4Address remoteAddr6 = appSink6->GetObject<Ipv4> ()->GetAddress (1,0).GetLocal ();



  OnOffHelper onoff6 ("ns3::TcpSocketFactory", Address (InetSocketAddress (remoteAddr6, 6)));
  onoff6.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
  onoff6.SetAttribute ("PacketSize", UintegerValue (1000));
  ApplicationContainer apps6= onoff6.Install (vanetnodes.Get (70));
  apps6.Start (Seconds (1));
  apps6.Stop (Seconds (1000));
  PacketSinkHelper sink6 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 6));
  apps6= sink6.Install (vanetnodes.Get (18));
 // apps6.Start (Seconds (1));

//7
  Ptr<Node> appSource7 = NodeList::GetNode (55);
  Ptr<Node> appSink7 = NodeList::GetNode (85);
  Ipv4Address remoteAddr7 = appSink7->GetObject<Ipv4> ()->GetAddress (1,0).GetLocal ();



  OnOffHelper onoff7 ("ns3::TcpSocketFactory", Address (InetSocketAddress (remoteAddr7, 7)));
  onoff7.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
  onoff7.SetAttribute ("PacketSize", UintegerValue (1000));
  ApplicationContainer apps7= onoff7.Install (vanetnodes.Get (55));
  apps7.Start (Seconds (1));
  apps7.Stop (Seconds (1000));
  PacketSinkHelper sink7 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 7));
  apps7= sink7.Install (vanetnodes.Get (85));
  //apps7.Start (Seconds (1));

//8
Ptr<Node> appSource8 = NodeList::GetNode (24);
  Ptr<Node> appSink8 = NodeList::GetNode (12);
  Ipv4Address remoteAddr8 = appSink8->GetObject<Ipv4> ()->GetAddress (1,0).GetLocal ();



  OnOffHelper onoff8 ("ns3::TcpSocketFactory", Address (InetSocketAddress (remoteAddr8, 8)));
  onoff8.SetAttribute ("DataRate", DataRateValue (DataRate ("100Mb/s")));
  onoff8.SetAttribute ("PacketSize", UintegerValue (1000));
  ApplicationContainer apps8= onoff8.Install (vanetnodes.Get (24));
  apps8.Start (Seconds (1));
  apps8.Stop (Seconds (1000));
  PacketSinkHelper sink8 ("ns3::TcpSocketFactory", InetSocketAddress (Ipv4Address::GetAny (), 8));
  apps8= sink8.Install (vanetnodes.Get (12));
 // apps8.Start (Seconds (1));

  wifiPhy.EnablePcap ("v2v25_s_c4_46",vanetnodes.Get(46)->GetId(),0);
  wifiPhy.EnablePcap ("v2v25_r_c4_60",vanetnodes.Get(60)->GetId(),0);
  wifiPhy.EnablePcap ("v2v25_s_c4_66",vanetnodes.Get(66)->GetId(),0);
  wifiPhy.EnablePcap ("v2v25_r_c4_2",vanetnodes.Get(2)->GetId(),0);
  wifiPhy.EnablePcap ("v2v25_s_c4_47 ",vanetnodes.Get(47)->GetId(),0);
  wifiPhy.EnablePcap ("v2v25_r_c4_56",vanetnodes.Get(56)->GetId(),0);
  wifiPhy.EnablePcap ("v2v25_s_c4_40",vanetnodes.Get(40)->GetId(),0);
  wifiPhy.EnablePcap ("v2v25_r_c4_14",vanetnodes.Get(14)->GetId(),0);
  wifiPhy.EnablePcap ("v2v25_s_c4_88 ",vanetnodes.Get(88)->GetId(),0);
  wifiPhy.EnablePcap ("v2v25_r_c4_32",vanetnodes.Get(32)->GetId(),0);
  wifiPhy.EnablePcap ("v2v25_s_c4_70",vanetnodes.Get(70)->GetId(),0);
  wifiPhy.EnablePcap ("v2v25_r_c4_18",vanetnodes.Get(18)->GetId(),0);
  wifiPhy.EnablePcap ("v2v25_s_c4_55 ",vanetnodes.Get(55)->GetId(),0);
  wifiPhy.EnablePcap ("v2v25_r_c4_85",vanetnodes.Get(85)->GetId(),0);
  wifiPhy.EnablePcap ("v2v25_s_c4_24",vanetnodes.Get(24)->GetId(),0);
  wifiPhy.EnablePcap ("v2v25_r_c4_12",vanetnodes.Get(12)->GetId(),0);



// Tracing
  //wifiPhy.EnablePcap ("vanet-aodv-mobility", vanetdevices);

// Installing Flow Monitor
  FlowMonitorHelper flowmonHelper;
  if (enableFlowMonitor)
    {
      flowmonHelper.InstallAll ();
    }
  Simulator::Stop (Seconds (1000));
  Simulator::Run ();

// Enabling the XML Flow Monitor
 if (enableFlowMonitor)
    {
      flowmonHelper.SerializeToXmlFile ("v2vc25_4.flowmon", false, false);
    }
 // AnimationInterface anim("8vegas-vanet-mobility1.xml");
  Simulator::Destroy ();
  return 0;
}
