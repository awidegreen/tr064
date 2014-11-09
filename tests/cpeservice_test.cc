// include global test defs
#include "cpe_test_global.hh"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <tr064/Service.hh>
#include <tr064/CpeElementBuilder.hh>

using namespace tr064;
using namespace ::testing;

//------------------------------------------------------------------------------

TEST(ServiceTest, ParseOk)
{
  const char* data = 
     "<service> \
           <serviceType>TheType</serviceType>\
           <serviceId>TheId</serviceId>\
           <controlURL>TheControlUrl</controlURL>\
           <eventSubURL>TheEventSubUrl</eventSubURL>\
           <SCPDURL>scpdurl</SCPDURL>\
         </service>";

  pugi::xml_document doc;
  auto result = doc.load(data);

  EXPECT_EQ( pugi::status_ok, result.status);

  pugi::xml_node xml = doc.child("service");
  EXPECT_FALSE( xml.empty() );

  MockCpeElementContainer mock_container;
  CpeElementBuilder builder(RESOUCRE_RETRIEVAL_EMPTY, &mock_container);

  EXPECT_CALL(mock_container, add_service(_) ).Times(1);

  auto service = builder.parse_cpe_service(xml);

  EXPECT_FALSE( service->empty() );
  EXPECT_EQ( "TheType", service->type() );
  EXPECT_EQ( "TheType", service->name() );
  EXPECT_EQ( "TheId", service->id() );
  EXPECT_EQ( "TheControlUrl", service->control_url() );
  EXPECT_EQ( "TheEventSubUrl", service->event_sub_url() );
  EXPECT_EQ( "scpdurl", service->scpd_url() );
}

//------------------------------------------------------------------------------

TEST(ServiceTest, ParseOkWithActions)
{
  const char* data = 
     "<service> \
        <serviceType>TheType</serviceType>\
        <serviceId>TheId</serviceId>\
        <controlURL>TheControlUrl</controlURL>\
        <eventSubURL>TheEventSubUrl</eventSubURL>\
        <SCPDURL>./wanipconnSCPD.xml</SCPDURL>\
      </service>";

  pugi::xml_document doc;
  auto result = doc.load(data);

  EXPECT_EQ( pugi::status_ok, result.status);

  pugi::xml_node xml = doc.child("service");
  EXPECT_FALSE( xml.empty() );

  MockCpeElementContainer mock_container;
  CpeElementBuilder builder(RESOUCRE_RETRIEVAL_FILE, &mock_container);

  EXPECT_CALL(mock_container, add_service(_) ).Times(1);
  EXPECT_CALL(mock_container, add_action(_) ).Times(2);

  auto service = builder.parse_cpe_service(xml);

  EXPECT_FALSE( service->empty() );

  EXPECT_EQ( "./wanipconnSCPD.xml", 
              service->scpd_url() );
  // check only the amount of parsed actions, the action parsing as such 
  // is done in the actions testcases (assume that it is working here)
  EXPECT_EQ( 2 , service->actions().size() );
}
