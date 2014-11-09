#include <gmock/gmock.h>
#include <gtest/gtest.h>

// tr064
#include <tr064/RootDevice.hh>
#include <tr064/Device.hh>
#include <tr064/Service.hh>
#include <tr064/ServiceAction.hh>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <sstream>

using namespace tr064;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  DEVICE tests
//------------------------------------------------------------------------------

TEST(DeserializeJsonTest, DeserializeDeviceParseOkWithService)
{
  std::string json = 
    "{ " 
    "  \"name\" : \"the name\", " \
    "  \"model_name\" : \"the model name\", " \
    "  \"model_description\" : \"the model description\", " \
    "  \"services\" : [ " \
    "     { \"name\" : \"foobar1\" }, "\
    "     { \"name\" : \"foobar2\" } "\
    "   ]" \
    "}";

  std::stringstream ss_json; ss_json << json;

  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss_json, pt);

  Device::Ptr dev(new Device());
  dev->deserialize_json(pt);

  EXPECT_EQ("the name", dev->name());
  EXPECT_EQ("the model name", dev->model_name());
  EXPECT_EQ("the model description", dev->model_description());
  EXPECT_EQ("", dev->friendly_name());

  EXPECT_EQ( 2, dev->services().size());
  EXPECT_EQ( "foobar1", dev->services()[0]->name());
  EXPECT_EQ( "foobar2", dev->services()[1]->name());
}

//------------------------------------------------------------------------------

TEST(DeserializeJsonTest, DeserializeDeviceParseOkWithSubdevices)
{
  std::string json = 
    "{ " 
    "  \"name\" : \"the name\", " \
    "  \"model_name\" : \"the model name\", " \
    "  \"model_description\" : \"the model description\", " \
    "  \"devices\" : [ " \
    "     { \"name\" : \"foobar1\" }, "\
    "     { \"name\" : \"foobar2\" } "\
    "   ]" \
    "}";

  std::stringstream ss_json; ss_json << json;

  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss_json, pt);

  Device::Ptr dev(new Device());
  dev->deserialize_json(pt);

  EXPECT_EQ("the name", dev->name());
  EXPECT_EQ("the model name", dev->model_name());
  EXPECT_EQ("the model description", dev->model_description());
  EXPECT_EQ("", dev->friendly_name());

  EXPECT_EQ( 0, dev->services().size());

  EXPECT_EQ( 2, dev->devices().size());
  EXPECT_EQ( "foobar1", dev->devices()[0]->name());
  EXPECT_EQ( "foobar2", dev->devices()[1]->name());
}

//------------------------------------------------------------------------------
TEST(DeserializeJsonTest, DeserializeDeviceParseOkWithoutService)
{
  std::string json = 
    "{ " 
    "  \"name\" : \"the name\", " \
    "  \"model_name\" : \"the model name\", " \
    "  \"model_description\" : \"the model description\" " \
    "}";

  std::stringstream ss_json; ss_json << json;

  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss_json, pt);

  Device::Ptr dev(new Device());
  dev->deserialize_json(pt);

  EXPECT_EQ("the name", dev->name());
  EXPECT_EQ("the model name", dev->model_name());
  EXPECT_EQ("the model description", dev->model_description());
  EXPECT_EQ("", dev->friendly_name());

  EXPECT_EQ( 0, dev->services().size());
}

//------------------------------------------------------------------------------

TEST(DeserializeJsonTest, DeserializeDeviceParseOkUnusedElementsInJson)
{
  std::string json = 
    "{ " 
    "  \"model_name\" : \"the model name\", " 
    "  \"friendly_name\" : \"friendly\", " 
    "  \"foo\" : \"bar\" " 
    "}";

  std::stringstream ss_json; ss_json << json;

  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss_json, pt);

  Device::Ptr dev(new Device());
  dev->deserialize_json(pt);

  EXPECT_EQ("", dev->name());
  EXPECT_EQ("the model name", dev->model_name());
  EXPECT_EQ("friendly", dev->friendly_name());

  EXPECT_EQ( 0, dev->services().size());
}

//------------------------------------------------------------------------------

TEST(DeserializeJsonTest, DeserializeDeviceParseNOkMalformated)
{
  std::string json = 
    "{ " 
    "  \"name\" : \"the name\", " 
    "  \"model_name\" : \"the model name\", " 
    "  \"model_description\" : \"the model description\", "  // HERE! ','
    "}";

  std::stringstream ss_json; ss_json << json;

  boost::property_tree::ptree pt;
  ASSERT_THROW( boost::property_tree::read_json(ss_json, pt), 
                std::exception);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  SERVICE tests
//------------------------------------------------------------------------------

TEST(DeserializeJsonTest, DeserializeServiceParseOkWithoutActions)
{
  std::string json = 
    "{ " 
    "  \"name\" : \"the name\", " 
    "  \"type\" : \"the type\", " 
    "  \"id\" : \"id?\", " 
    "  \"control_url\" : \"the ctrl url\", " 
    "  \"event_subscription_url\" : \"the event sub url\", " 
    "  \"scpd_url\" : \"scpturl?\" " 
    "}";

  std::stringstream ss_json; ss_json << json;

  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss_json, pt);

  Service::Ptr s(new Service());
  s->deserialize_json(pt);

  EXPECT_EQ("the name", s->name());
  EXPECT_EQ("the type", s->type());
  EXPECT_EQ("id?", s->id());
  EXPECT_EQ("the ctrl url", s->control_url());
  EXPECT_EQ("the event sub url", s->event_sub_url());
  EXPECT_EQ("scpturl?", s->scpd_url());

  EXPECT_EQ( 0, s->actions().size());
}

//------------------------------------------------------------------------------

TEST(DeserializeJsonTest, DeserializeServiceParseOkWithActions)
{
  std::string json = 
    "{ " 
    "  \"name\" : \"the name\", " 
    "  \"type\" : \"the type\", " 
    "  \"actions\" : [ " \
    "     { \"name\" : \"foobar1\" }, "\
    "     { \"name\" : \"foobar2\" } "\
    "   ]" \
    "}";

  std::stringstream ss_json; ss_json << json;

  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss_json, pt);

  Service::Ptr s(new Service());
  s->deserialize_json(pt);

  EXPECT_EQ("the name", s->name());
  EXPECT_EQ("the type", s->type());
  EXPECT_EQ("", s->id());
  EXPECT_EQ("", s->control_url());
  EXPECT_EQ("", s->event_sub_url());
  EXPECT_EQ("", s->scpd_url());

  EXPECT_EQ( 2, s->actions().size());
  EXPECT_EQ( "foobar1", s->actions()[0]->name());
  EXPECT_EQ( "foobar2", s->actions()[1]->name());
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  ACTION tests
//------------------------------------------------------------------------------

TEST(DeserializeJsonTest, DeserializeActionParseOkWithArguments)
{
  std::string json = 
    "{ " 
    "  \"name\" : \"the name\", " 
    "  \"arguments\" : [ " \
    "     { "
    "       \"name\" : \"arg1\" , "
    "       \"direction\" : \"in\" "
    "     },"
    "     { "
    "       \"name\" : \"arg2\" , "
    "       \"direction\" : \"out\" "
    "     },"
    "     { "
    "       \"name\" : \"arg3\" "
    "     }"
    "   ]" 
    "}";

  std::stringstream ss_json; ss_json << json;

  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss_json, pt);

  ServiceAction::Ptr a(new ServiceAction());
  a->deserialize_json(pt);

  EXPECT_EQ("the name", a->name());

  EXPECT_EQ( 3, a->args().size() );
  EXPECT_EQ( "arg1", a->args()[0]->name());
  EXPECT_EQ( ServiceActionArg::IN, a->args()[0]->direction());
  EXPECT_EQ( "arg2", a->args()[1]->name());
  EXPECT_EQ( ServiceActionArg::OUT, a->args()[1]->direction());
  EXPECT_EQ( "arg3", a->args()[2]->name());
  EXPECT_EQ( ServiceActionArg::IN, a->args()[2]->direction());
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  RootDevice tests
//------------------------------------------------------------------------------

TEST(DeserializeJsonTest, DeserializeRootDevicenParseOk)
{
  std::string json = 
    "{ " 
    "  \"server\" : \"the server\", " 
    "  \"location\" : \"the loc\", " 
    "  \"usn\" : \"the usn\", " 
    "  \"device\" :  " \
    "     { "
    "       \"name\" : \"cpe device name\" "
    "     }"
    "}";

  std::stringstream ss_json; ss_json << json;

  boost::property_tree::ptree pt;
  boost::property_tree::read_json(ss_json, pt);

  RootDevice::Ptr rd(new RootDevice());
  rd->deserialize_json(pt);

  EXPECT_EQ("the server", rd->server());
  EXPECT_EQ("the loc", rd->location());
  EXPECT_EQ("the usn", rd->usn());
  EXPECT_EQ("cpe device name", rd->cpe_device()->name());
}
