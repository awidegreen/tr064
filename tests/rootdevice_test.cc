// include global test defs
#include "cpe_test_global.hh"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <tr064/Device.hh>
#include <tr064/RootDevice.hh>

#include <tr064/CpeElementBuilder.hh>

#include <string>

using namespace tr064;
using namespace ::testing;

namespace 
{
  const std::string location = "the loc";
  const std::string server = "the server";
  const std::string cache_ctrl = "the cache ctrl";
  const std::string s_type = "the s type";
  const std::string usn = "the usn";
  const std::string device_desc_doc = "the dev desc doc";
  const std::string host = "the host";
  const std::string port = "the port";
  const std::string cpe_dev_name = "the cpe device";
  const std::string cpe_dev_model = "the cpe device model";
}

//------------------------------------------------------------------------------

TEST(RootDeviceTest, GetSubDevices)
{
  auto dev = new Device();
  
  Device::Ptr sub1(new Device());
  dev->add_device(sub1);

  Device::Ptr sub2(new Device());
  Device::Ptr subsub1(new Device());
  Device::Ptr subsub2(new Device());
  sub2->add_device(subsub1);
  sub2->add_device(subsub2);
  dev->add_device(sub2);

  RootDevice root(location, server, cache_ctrl, s_type, usn, 
                  device_desc_doc, host, port, dev);

  EXPECT_EQ(5, root.devices().size());
}
