#include <tr064/RootDeviceDiscovery.hh>

#include "Tr064Exception.hh"

// boost
#include <boost/array.hpp>
#include <boost/asio.hpp>

// std lib
#include <iostream>

//------------------------------------------------------------------------------

namespace 
{
  //------------------------------
  static const char* DISCOVERY_MESSAGE = 
    "M-SEARCH * HTTP/1.1\n"
    "Host: 239.255.255.250:1900\r\n"
    "Man: \"ssdp:discover\"\r\n"
    "MX: 5\r\n"
    "ST: urn:dslforum-org:device:InternetGatewayDevice:1\r\n";
  static const char* SSDP_GRP  = "239.255.255.250";
  static const int   SSDP_PORT = 1900;
  //------------------------------
  

}

//------------------------------------------------------------------------------

using namespace tr064;

//------------------------------------------------------------------------------

RootDevice::Vector
RootDeviceDiscovery::discover()
{
  RootDevice::Vector devices;

  using namespace boost::asio::ip;

  ReceiveBuffer recv_buf;
  udp::endpoint sender_endpoint;
  size_t len;

  try
  {
    // TODO: we should iterate over every response,
    // since the discovery is a multicast request (consult with boost docs)
    boost::asio::io_service io_service;


    udp::endpoint endpoint(address::from_string(SSDP_GRP), SSDP_PORT);

    udp::socket socket(io_service);
    socket.open(udp::v4());

    socket.send_to(
        boost::asio::buffer(
          DISCOVERY_MESSAGE, strlen(DISCOVERY_MESSAGE)), endpoint);

    len = socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    throw Tr064Exception("Error during RootDevice Discovery: ");
  }

  std::cout << "RootDevice found at " << sender_endpoint.address() << std::endl;

  auto device = 
    RootDevice::create_root_device_from_http_buf(recv_buf.data(), len);
  devices.push_back(std::move(device));

  return devices;
}

//------------------------------------------------------------------------------

