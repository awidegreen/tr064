// include global test defs
#include "cpe_test_global.hh"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <Utils.hh>

TEST(UtilsTest, ExtractNameFromTypeOk)
{
  std::string type = "urn:dslforum-org:device:InternetGatewayDevice:1";

  auto r = extract_type_name(type);

  EXPECT_EQ( "InternetGatewayDevice", r );
}

//------------------------------------------------------------------------------

TEST(UtilsTest, ExtractNameFromTypeNotOk)
{
  std::string type = "urn:dslforum-org:device:InternetGatewayDevice";

  auto r = extract_type_name(type);

  EXPECT_NE( "InternetGatewayDevice", r );
  EXPECT_EQ( "device", r );

  type = "foobar";
  r = extract_type_name(type);
  EXPECT_EQ( "foobar", r );
}
