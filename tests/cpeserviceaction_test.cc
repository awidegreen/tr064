// include global test defs
#include "cpe_test_global.hh"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <tr064/CpeElementBuilder.hh>
#include <tr064/ServiceAction.hh>

using namespace tr064;
using namespace ::testing;

//------------------------------------------------------------------------------

TEST(ServiceActionTest, ParseOk)
{
  const char* data = 
  "<action>\
     <name>GetConnectionTypeInfo</name>\
     <argumentList>\
        <argument>\
           <name>NewConnectionType</name>\
           <direction>in</direction>\
           <relatedStateVariable>ConnectionType</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>NewPossibleConnectionTypes</name>\
          <direction>out</direction>\
          <relatedStateVariable>PossibleConnectionTypes</relatedStateVariable>\
        </argument>\
        <argument>\
          <name>Foobar</name>\
        </argument>\
      </argumentList>\
    </action>";

  pugi::xml_document doc;
  auto result = doc.load(data);

  EXPECT_EQ( pugi::status_ok, result.status);

  pugi::xml_node xml = doc.child("action");
  EXPECT_FALSE( xml.empty() );

  MockCpeElementContainer mock_container;
  CpeElementBuilder builder(RESOUCRE_RETRIEVAL_EMPTY, &mock_container);

  EXPECT_CALL(mock_container, add_device(_) ).Times(0);
  EXPECT_CALL(mock_container, add_service(_) ).Times(0);
  EXPECT_CALL(mock_container, add_action(_) ).Times(1);

  auto action = builder.parse_cpe_service_action(xml);

  EXPECT_FALSE( action->empty() );

  EXPECT_EQ( 3, action->args().size() );

  //auto arg = action->get_arg("NewConnectionType");
  //EXPECT_TRUE( arg.found );
  //EXPECT_EQ( ServiceActionArg::IN, arg.element->direction() );
  //// TODO state var

  //arg = action->get_arg("NewPossibleConnectionTypes");
  //EXPECT_TRUE( arg.found );
  //EXPECT_EQ( ServiceActionArg::OUT, arg.element->direction() );
  //// TODO state var

  //arg = action->get_arg("Foobar");
  //EXPECT_TRUE( arg.found );
  //EXPECT_EQ( ServiceActionArg::NOT_DEFINED, arg.element->direction() );
  //// TODO state var

  //// shall not exsit
  //arg = action->get_arg("Bar");
  //EXPECT_FALSE( arg.found );
}

//------------------------------------------------------------------------------

TEST(ServiceActionTest, Empty)
{
  const char* data = 
  "<action>\
    </action>";

  pugi::xml_document doc;
  auto result = doc.load(data);

  EXPECT_EQ( pugi::status_ok, result.status);
  pugi::xml_node xml = doc.child("action");

  MockCpeElementContainer mock_container;
  CpeElementBuilder builder(RESOUCRE_RETRIEVAL_EMPTY, &mock_container);

  EXPECT_CALL(mock_container, add_device(_) ).Times(0);
  EXPECT_CALL(mock_container, add_service(_) ).Times(0);
  EXPECT_CALL(mock_container, add_action(_) ).Times(1);

  auto action = builder.parse_cpe_service_action(xml);
  EXPECT_TRUE( action->empty() );
}

