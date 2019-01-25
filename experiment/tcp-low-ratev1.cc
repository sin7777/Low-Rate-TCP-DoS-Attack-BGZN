/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
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
 * Implementation Done By:
 * Adarsh Honawad <adarsh2397@gmail.com>
 * Sagar Bharadwaj <sagarbharadwaj50@gmail.com>
 * Samvid Dharanikota <samvid25@yahoo.com>
 */

/* 
 * The topology used to simulate this attack contains 5 nodes as follows:
 * n0 -> alice (sender)
 * n1 -> eve (attacker)
 * n2 -> router (common router between alice and eve)
 * n3 -> router (router conneced to bob)
 * n4 -> bob (receiver)

     n1
        \ pp1 (100 Mbps, 2ms RTT)
         \
          \             -> pp1 (100 Mbps, 2ms RTT)
           \            |
            n2 ---- n3 ---- n4
           /    |
          /     -> pp2 (1.5 Mbps, 40ms RTT)
         /
        / pp1 (100 Mbps, 2ms RTT)
     n0

*/

#include "ns3/nstime.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/traffic-control-module.h"
#include <cstring>
#include <cstdlib>


#define TCP_SINK_PORT 9000
#define UDP_SINK_PORT 9001

// Experimentation parameters
#define BULK_SEND_MAX_BYTES 20971520
//#define BULK_SEND_MAX_BYTES 4097152

#define MAX_SIMULATION_TIME 100.0
#define ATTACKER_START 0.0
#define ATTACKER_RATE (std::string)"7000kb/s"
//#define ATTACKER_RATE (std::string) "0kb/s"

#define ON_TIME (std::string)"0.25"
//#define ON_TIME (std::string) "2"
#define BURST_PERIOD 1
#define OFF_TIME std::to_string(BURST_PERIOD - stof(ON_TIME))
#define SENDER_START 0.75 //Must be equal to OFF_TIME

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("FirstScriptExample");

int main(int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse(argc, argv);

  Time::SetResolution(Time::NS);
  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

  std::string redLinkDataRate = "100Mbps";
  std::string redLinkDelay = "2ms";

  NodeContainer nodes;
  //nodes.Create(6);
  nodes.Create(7);

  // Default TCP is TcpNewReno (no need to change, unless experimenting with other variants)
  Config::SetDefault("ns3::TcpL4Protocol::SocketType", StringValue("ns3::TcpNewReno"));
		  
  // Define the Point-to-Point links (helpers) and their paramters
  PointToPointHelper pp1, pp2;
  pp1.SetDeviceAttribute("DataRate", StringValue("100Mbps"));
  pp1.SetChannelAttribute("Delay", StringValue("1ms"));
  
  // Add a DropTailQueue to the bottleneck link
  pp2.SetQueue("ns3::DropTailQueue", "MaxPackets", UintegerValue(1));
  pp2.SetDeviceAttribute("DataRate", StringValue("1.5Mbps"));
  pp2.SetChannelAttribute("Delay", StringValue("20ms"));

  NetDeviceContainer d02, d12, d23, d34, d52, d62;
  //NetDeviceContainer d02, d12, d23, d34;
  //NetDeviceContainer d02, d12, d23, d34, d25;
  d02 = pp1.Install(nodes.Get(0), nodes.Get(2));
  d12 = pp1.Install(nodes.Get(1), nodes.Get(2));
  d23 = pp2.Install(nodes.Get(2), nodes.Get(3));
  d34 = pp1.Install(nodes.Get(3), nodes.Get(4));
  //d25 = pp1.Install(nodes.Get(2), nodes.Get(5));

  d52 = pp1.Install(nodes.Get(2),nodes.Get(5));
  d62 = pp1.Install(nodes.Get(2),nodes.Get(6));



  //d02.SetAttribute("ReceiveErrorModel",DoubleValue(0.1));

  TrafficControlHelper tchRed;
  tchRed.SetRootQueueDisc("ns3::RedQueueDisc",
		  "LinkBandwidth",StringValue(redLinkDataRate),
		  "LinkDelay",StringValue(redLinkDelay),
		  "QueueLimit",UintegerValue(1000),
		  "Gentle",BooleanValue(false),
		  "Wait",BooleanValue(false));

  TrafficControlHelper tchPfifo;
  uint16_t handle = tchPfifo.SetRootQueueDisc("ns3::PfifoFastQueueDisc");
  tchPfifo.AddInternalQueues (handle,3,"ns3::DropTailQueue","MaxPackets",UintegerValue(1000));
  
  InternetStackHelper stack;
  stack.Install(nodes);

  //QueueDiscContainer queueDiscs = tchRed.Install(d02);
  //QueueDiscContainer queueDiscs1 = tchRed.Install(d52);
  //QueueDiscContainer queueDiscs3 = tchRed.Install(d23);
  //QueueDiscContainer queueDiscs2 = tchRed.Install(d62);
  //QueueDiscContainer queueDiscs4 = tchRed.Install(d34);

  //tchPfifo.Install(d02);
  //tchPfifo.Install(d23);
  //tchPfifo.Install(d52);
  //tchPfifo.Install(d62);
  //tchPfifo.Install(d34);
  
  Ipv4AddressHelper a02, a12, a23, a34, a52, a62;
  //Ipv4AddressHelper a02, a12, a23, a34;
  //Ipv4AddressHelper a02, a12, a23, a34, a25;
  a02.SetBase("10.1.1.0", "255.255.255.0");
  a12.SetBase("10.1.2.0", "255.255.255.0");
  a23.SetBase("10.1.3.0", "255.255.255.0");
  a34.SetBase("10.1.4.0", "255.255.255.0");
  //a25.SetBase("10.1.5.0", "255.255.255.0");

  a52.SetBase("10.1.5.0", "255.255.255.0");
  a62.SetBase("10.1.6.0", "255.255.255.0");



  Ipv4InterfaceContainer i02, i12, i23, i34, i52, i62;
  //Ipv4InterfaceContainer i02, i12, i23, i34;
  //Ipv4InterfaceContainer i02, i12, i23, i34, i25;
  i02 = a02.Assign(d02);
  i12 = a12.Assign(d12);
  i23 = a23.Assign(d23);
  i34 = a34.Assign(d34);
  //i25 = a25.Assign(d25);
  i52 = a52.Assign(d52);
  i62 = a62.Assign(d62);



  // UDP On-Off Application - Application used by attacker (eve) to create the low-rate bursts.
  OnOffHelper onoff("ns3::UdpSocketFactory",
                    Address(InetSocketAddress(i34.GetAddress(1), UDP_SINK_PORT)));
  onoff.SetConstantRate(DataRate(ATTACKER_RATE));
  onoff.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=" + ON_TIME + "]"));
  onoff.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=" + OFF_TIME + "]"));
  ApplicationContainer onOffApp = onoff.Install(nodes.Get(1));
  onOffApp.Start(Seconds(ATTACKER_START));
  onOffApp.Stop(Seconds(MAX_SIMULATION_TIME));
  

  /*OnOffHelper onoff1("ns3::UdpSocketFactory",
                    Address(InetSocketAddress(i34.GetAddress(1), UDP_SINK_PORT)));
  onoff1.SetConstantRate(DataRate((std::string)"3500kb/s"));
  onoff1.SetAttribute("OnTime", StringValue("ns3::ConstantRandomVariable[Constant=" + (std::string)"2" + "]"));
  onoff1.SetAttribute("OffTime", StringValue("ns3::ConstantRandomVariable[Constant=" + (std::string)"3" + "]"));
  ApplicationContainer onOffApp1 = onoff1.Install(nodes.Get(5));
  onOffApp1.Start(Seconds(0.0));
  onOffApp1.Stop(Seconds(100.0));
*/


  // TCP Bulk Send Application - Application used by the legit node (alice) to send data to a receiver.
  BulkSendHelper bulkSend("ns3::TcpSocketFactory",
                          InetSocketAddress(i34.GetAddress(1), TCP_SINK_PORT));
  bulkSend.SetAttribute("MaxBytes", UintegerValue(BULK_SEND_MAX_BYTES));
  ApplicationContainer bulkSendApp = bulkSend.Install(nodes.Get(0));
  bulkSendApp.Start(Seconds(SENDER_START));
  bulkSendApp.Stop(Seconds(MAX_SIMULATION_TIME));
  
  BulkSendHelper bulkSend1("ns3::TcpSocketFactory",
                          InetSocketAddress(i34.GetAddress(1), TCP_SINK_PORT));
  bulkSend1.SetAttribute("MaxBytes", UintegerValue(BULK_SEND_MAX_BYTES));
  ApplicationContainer bulkSendApp1 = bulkSend1.Install(nodes.Get(5));
  bulkSendApp1.Start(Seconds(SENDER_START));
  bulkSendApp1.Stop(Seconds(MAX_SIMULATION_TIME));
  
  BulkSendHelper bulkSend2("ns3::TcpSocketFactory",
                          InetSocketAddress(i34.GetAddress(1), TCP_SINK_PORT));
  bulkSend2.SetAttribute("MaxBytes", UintegerValue(BULK_SEND_MAX_BYTES));
  ApplicationContainer bulkSendApp2 = bulkSend2.Install(nodes.Get(6));
  bulkSendApp2.Start(Seconds(SENDER_START));
  bulkSendApp2.Stop(Seconds(MAX_SIMULATION_TIME));
  

  // UDP sink on the receiver (bob).
  PacketSinkHelper UDPsink("ns3::UdpSocketFactory",
                           Address(InetSocketAddress(Ipv4Address::GetAny(), UDP_SINK_PORT)));
  ApplicationContainer UDPSinkApp = UDPsink.Install(nodes.Get(4));
  UDPSinkApp.Start(Seconds(0.0));
  UDPSinkApp.Stop(Seconds(MAX_SIMULATION_TIME));


  // TCP sink on the receiver (bob).
  PacketSinkHelper TCPsink("ns3::TcpSocketFactory",
                           InetSocketAddress(Ipv4Address::GetAny(), TCP_SINK_PORT));
  ApplicationContainer TCPSinkApp = TCPsink.Install(nodes.Get(4));
  
  TCPSinkApp.Start(Seconds(0.0));
  TCPSinkApp.Stop(Seconds(MAX_SIMULATION_TIME));
   Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  pp1.EnablePcapAll("PCAPs/tcplow");
  
  Simulator::Run();
  Simulator::Destroy();
  return 0;
}
