#include <gmock/gmock.h>
#include <gtest/gtest.h>

// tr064
#include <tr064/RootDevice.hh>
#include <tr064/Device.hh>
#include <tr064/Service.hh>
#include <tr064/ServiceAction.hh>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

using namespace tr064;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  DEVICE tests
//------------------------------------------------------------------------------

TEST(SerializeJsonTest, SerializeDeviceOkWithoutChildren)
{
  std::vector<std::string> dev_props { "the name", "the model name", "the model desc", "the friendly name"};
  Device dev(dev_props[0], dev_props[1], dev_props[2], dev_props[3]);

  boost::property_tree::ptree parent;

  dev.serialize_json(parent);

  std::stringstream ss;
  boost::property_tree::json_parser::write_json(ss, parent);

  auto to_serialize = ss.str();

  EXPECT_NE(std::string::npos, to_serialize.find("name"));
  EXPECT_EQ(std::string::npos, to_serialize.find("services"));
  EXPECT_EQ(std::string::npos, to_serialize.find("devices"));

  for (auto& p : dev_props )
  {
    EXPECT_NE(std::string::npos, to_serialize.find(p));
  }
}

//------------------------------------------------------------------------------

TEST(SerializeJsonTest, SerializeDeviceOkWithChildren)
{
  std::vector<std::string> dev_props { "the name", "the model name", "the model desc", "the friendly name"};
  Device dev(dev_props[0], dev_props[1], dev_props[2], dev_props[3]);

  std::vector<std::string> s1_props {
        "s1 type", 
        "s1 name", 
        "s1 id", 
        "s1 control url", 
        "s1 evne url",
        "s1 scpd url"};
  Service::Ptr service1(
      new Service(s1_props[0], s1_props[1], s1_props[2], s1_props[3], s1_props[4], s1_props[5]));
  dev.add_service(service1);

  std::vector<std::string> s2_props {
        "s2 type", 
        "s2 name", 
        "s2 id", 
        "s2 control url", 
        "s2 evne url",
        "s2 scpd url"};
  Service::Ptr service2(
      new Service(s2_props[0], s2_props[1], s2_props[2], s2_props[3], s2_props[4], s2_props[5]));
  dev.add_service(service2);

  std::vector<std::string> sub_dev_props { "subdev the name", "subdev the model_name", "subdev the model desc", "subdev the friendly name"};
  Device::Ptr sub_dev(new Device(sub_dev_props[0], sub_dev_props[1], sub_dev_props[2], sub_dev_props[3]));
  dev.add_device(sub_dev);

  boost::property_tree::ptree parent;

  dev.serialize_json(parent);

  std::stringstream ss;
  boost::property_tree::json_parser::write_json(ss, parent);

  auto to_serialize = ss.str();

  EXPECT_NE(std::string::npos, to_serialize.find("name"));
  EXPECT_NE(std::string::npos, to_serialize.find("services"));
  EXPECT_NE(std::string::npos, to_serialize.find("devices"));

  for (auto& p : dev_props )
  {
    EXPECT_NE(std::string::npos, to_serialize.find(p));
  }
  for (auto& p : s1_props )
  {
    EXPECT_NE(std::string::npos, to_serialize.find(p));
  }
  for (auto& p : s2_props )
  {
    EXPECT_NE(std::string::npos, to_serialize.find(p));
  }
  for (auto& p : sub_dev_props )
  {
    EXPECT_NE(std::string::npos, to_serialize.find(p));
  }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  SERVICE tests
//------------------------------------------------------------------------------

TEST(SerializeJsonTest, SerializeServiceOkWithChildren)
{
  const std::string s_type = "s1 type";
  const std::string s_name = "s1 name";
  const std::string s_id = "s1 id";
  const std::string s_ctrl_url = "s1 control url";
  const std::string s_event_url = "s1 event url";
  const std::string s_scpd_url = "s1 scpd url";
  const std::string a1_name = "action 1";
  const std::string a2_name = "action 2";

  Service s(
        s_type, 
        s_name, 
        s_id, 
        s_ctrl_url, 
        s_event_url,
        s_scpd_url);
  ServiceAction::Ptr action1(new ServiceAction(a1_name));
  s.add_action(action1);
  ServiceAction::Ptr action2(new ServiceAction(a2_name));
  s.add_action(action2);

  boost::property_tree::ptree parent;

  s.serialize_json(parent);

  std::stringstream ss;
  boost::property_tree::json_parser::write_json(ss, parent);

  auto to_serialize = ss.str();

  EXPECT_NE(std::string::npos, to_serialize.find("name"));
  EXPECT_NE(std::string::npos, to_serialize.find(s_type));
  EXPECT_NE(std::string::npos, to_serialize.find(s_name));
  EXPECT_NE(std::string::npos, to_serialize.find(s_id));
  EXPECT_NE(std::string::npos, to_serialize.find(s_ctrl_url));
  EXPECT_NE(std::string::npos, to_serialize.find(s_event_url));
  EXPECT_NE(std::string::npos, to_serialize.find(s_scpd_url));
  EXPECT_NE(std::string::npos, to_serialize.find("actions"));
  EXPECT_NE(std::string::npos, to_serialize.find(a1_name));
  EXPECT_NE(std::string::npos, to_serialize.find(a2_name));
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  ACTION tests
//------------------------------------------------------------------------------

TEST(SerializeJsonTest, SerializeActionOkWithChildren)
{
  const std::string a_name = "action 1";
  const std::string arg1_name = "arg 1 name";
  const std::string arg2_name = "argj2 name";

  ServiceAction a(a_name);
  ServiceActionArg::Ptr arg1(new ServiceActionArg(arg1_name, ServiceActionArg::IN));
  a.add_arg(arg1);
  ServiceActionArg::Ptr arg2(new ServiceActionArg(arg2_name, ServiceActionArg::OUT));
  a.add_arg(arg2);

  boost::property_tree::ptree parent;

  a.serialize_json(parent);

  std::stringstream ss;
  boost::property_tree::json_parser::write_json(ss, parent);

  auto to_serialize = ss.str();

  EXPECT_NE(std::string::npos, to_serialize.find("name"));
  EXPECT_NE(std::string::npos, to_serialize.find(a_name));
  EXPECT_NE(std::string::npos, to_serialize.find("arguments"));
  EXPECT_NE(std::string::npos, to_serialize.find(arg1_name));
  EXPECT_NE(std::string::npos, to_serialize.find(arg2_name));
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  RootDevice tests
//------------------------------------------------------------------------------

TEST(SerializeJsonTest, SerializeRootDeviceOk)
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

  auto dev = new Device(cpe_dev_name, cpe_dev_model);
  RootDevice rd(location, server, cache_ctrl, s_type, usn, device_desc_doc, host, port, dev);

  boost::property_tree::ptree parent;

  rd.serialize_json(parent);

  std::stringstream ss;
  boost::property_tree::json_parser::write_json(ss, parent);

  auto to_serialize = ss.str();

  EXPECT_NE(std::string::npos, to_serialize.find(location));
  EXPECT_NE(std::string::npos, to_serialize.find(server));
  EXPECT_NE(std::string::npos, to_serialize.find(cache_ctrl));
  EXPECT_NE(std::string::npos, to_serialize.find(s_type));
  EXPECT_NE(std::string::npos, to_serialize.find(usn));
  EXPECT_NE(std::string::npos, to_serialize.find(device_desc_doc));
  EXPECT_NE(std::string::npos, to_serialize.find(host));
  EXPECT_NE(std::string::npos, to_serialize.find(port));
  EXPECT_NE(std::string::npos, to_serialize.find(cpe_dev_name));
  EXPECT_NE(std::string::npos, to_serialize.find(cpe_dev_model));
}
