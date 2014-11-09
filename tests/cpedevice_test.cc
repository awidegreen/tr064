// include global test defs
#include "cpe_test_global.hh"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <tr064/Device.hh>
#include <tr064/CpeElementBuilder.hh>

using namespace tr064;
using namespace ::testing;


//------------------------------------------------------------------------------

TEST(DeviceTest, Empty)
{
  Device::Ptr dev(new Device());

  EXPECT_TRUE( dev->empty() );

}

//------------------------------------------------------------------------------
TEST(DeviceTest, GetSubDevices)
{
  Device dev;
  
  Device::Ptr sub1(new Device());
  dev.add_device(sub1);

  Device::Ptr sub2(new Device());
  Device::Ptr subsub1(new Device());
  Device::Ptr subsub2(new Device());
  sub2->add_device(subsub1);
  sub2->add_device(subsub2);
  dev.add_device(sub2);

  EXPECT_EQ(4, dev.devices().size());
}

//------------------------------------------------------------------------------

TEST(DeviceTest, ParseOk)
{
  const char* data = 
     "<device> \
        <deviceType>type</deviceType> \
        <friendlyName>friendly name</friendlyName> \
        <manufacturer>AVM</manufacturer>\
        <manufacturerURL>www.avm.de</manufacturerURL>\
        <serviceList> \
         <service> \
           <serviceType>TheType</serviceType>\
           <serviceId>TheId</serviceId>\
           <controlURL>TheControlUrl</controlURL>\
           <eventSubURL>TheEventSubUrl</eventSubURL>\
           <SCPDURL>scpdurl</SCPDURL>\
         </service> \
        </serviceList> \
      </device>";

  pugi::xml_document doc;
  auto result = doc.load(data);

  EXPECT_EQ( pugi::status_ok, result.status);

  pugi::xml_node xml = doc.child("device");
  EXPECT_FALSE( xml.empty() );

  MockCpeElementContainer mock_container;
  CpeElementBuilder builder(RESOUCRE_RETRIEVAL_EMPTY, &mock_container);

  EXPECT_CALL(mock_container, add_device(_) ).Times(1);
  EXPECT_CALL(mock_container, add_service(_) ).Times(1);
  EXPECT_CALL(mock_container, add_action(_) ).Times(0);

  auto device = builder.parse_cpe_device(xml);

  EXPECT_FALSE( device->empty() );

  EXPECT_EQ( "friendly name", device->friendly_name());
  EXPECT_EQ( "type", device->name());
}

//------------------------------------------------------------------------------

TEST(DeviceTest, ParseNOk_NoDeviceInXML)
{
  const char* data = 
     "  <deviceType>type</deviceType> \
        <friendlyName>friendly name</friendlyName> \
        <manufacturer>AVM</manufacturer>\
        <manufacturerURL>www.avm.de</manufacturerURL>\
      ";

  pugi::xml_document doc;
  auto result = doc.load(data);
  EXPECT_EQ( pugi::status_ok, result.status);

  pugi::xml_node xml = doc.child("device");
  EXPECT_TRUE( xml.empty() );

  MockCpeElementContainer mock_container;
  CpeElementBuilder builder(RESOUCRE_RETRIEVAL_EMPTY, &mock_container);

  EXPECT_CALL(mock_container, add_device(_) ).Times(0);
  EXPECT_CALL(mock_container, add_service(_) ).Times(0);
  EXPECT_CALL(mock_container, add_action(_) ).Times(0);

  auto device = builder.parse_cpe_device(xml);

  EXPECT_TRUE( device->empty() );
}

//------------------------------------------------------------------------------

TEST(DeviceTest, ParseNOk_NoPropsInDevice)
{

  const char* data = 
     "<device> \
      </device>";

  pugi::xml_document doc;
  auto result = doc.load(data);
  EXPECT_EQ( pugi::status_ok, result.status);

  pugi::xml_node xml = doc.child("device");
  EXPECT_FALSE( xml.empty() );

  MockCpeElementContainer mock_container;
  CpeElementBuilder builder(RESOUCRE_RETRIEVAL_EMPTY, &mock_container);

  EXPECT_CALL(mock_container, add_device(_) ).Times(1);
  EXPECT_CALL(mock_container, add_service(_) ).Times(0);
  EXPECT_CALL(mock_container, add_action(_) ).Times(0);

  auto device = builder.parse_cpe_device(xml);

  EXPECT_TRUE( device->empty() );

}

//------------------------------------------------------------------------------

TEST(DeviceTest, ParseOkWithService)
{
  const char* data = 
     "<device> \
        <deviceType>type</deviceType> \
        <friendlyName>friendly name</friendlyName> \
        <manufacturer>AVM</manufacturer>\
        <manufacturerURL>www.avm.de</manufacturerURL>\
        <serviceList> \
         <service> \
           <serviceType>42</serviceType>\
           <serviceId>TheId</serviceId>\
           <controlURL>TheControlUrl</controlURL>\
           <eventSubURL>TheEventSubUrl</eventSubURL>\
           <SCPDURL>scpdurl</SCPDURL>\
         </service> \
        </serviceList> \
      </device>";

  pugi::xml_document doc;
  auto result = doc.load(data);

  EXPECT_EQ( pugi::status_ok, result.status);

  pugi::xml_node xml = doc.child("device");
  EXPECT_FALSE( xml.empty() );

  MockCpeElementContainer mock_container;
  CpeElementBuilder builder(RESOUCRE_RETRIEVAL_EMPTY, &mock_container);

  EXPECT_CALL(mock_container, add_device(_) ).Times(1);
  EXPECT_CALL(mock_container, add_service(_) ).Times(1);
  EXPECT_CALL(mock_container, add_action(_) ).Times(0);

  auto device = builder.parse_cpe_device(xml);

  EXPECT_FALSE( device->empty() );

  EXPECT_EQ( "friendly name", device->friendly_name());

  EXPECT_EQ( 1, device->services().size() );
  auto& s_p_result = device->services().front()->type();
  EXPECT_EQ( "42", s_p_result);
}

//------------------------------------------------------------------------------t

TEST(DeviceTest, ParseOkWithSubDevice)
{
  const char* data = 
     "<device> \
        <deviceType>the_dev</deviceType> \
        <deviceList> \
         <device> \
           <deviceType>the__sub_dev</deviceType> \
         </device> \
        </deviceList> \
      </device>";

  pugi::xml_document doc;
  auto result = doc.load(data);

  EXPECT_EQ( pugi::status_ok, result.status);

  pugi::xml_node xml = doc.child("device");
  EXPECT_FALSE( xml.empty() );

  MockCpeElementContainer mock_container;
  CpeElementBuilder builder(RESOUCRE_RETRIEVAL_EMPTY, &mock_container);

  EXPECT_CALL(mock_container, add_device(_) ).Times(2);
  EXPECT_CALL(mock_container, add_service(_) ).Times(0);
  EXPECT_CALL(mock_container, add_action(_) ).Times(0);

  auto device = builder.parse_cpe_device(xml);
  EXPECT_FALSE( device->empty() );

  EXPECT_EQ( "the_dev", device->name() );

  EXPECT_EQ( 0, device->services().size() );

  EXPECT_EQ( 1, device->devices().size() );
  auto sub_dev = device->devices().front();
  EXPECT_EQ( "the__sub_dev", sub_dev->name());
}
