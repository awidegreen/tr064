#ifndef ROOTDEVICEDISCOVERY_HH
#define ROOTDEVICEDISCOVERY_HH

#include "RootDevice.hh"

// boost
#include <boost/array.hpp>

// std lib
#include <memory>

namespace tr064
{

class RootDeviceDiscovery
{
public:
  RootDevice::Vector discover();

private:
  typedef boost::array<char, 4096> ReceiveBuffer;
};

}

#endif /* ROOTDEVICEDISCOVERY_HH */
