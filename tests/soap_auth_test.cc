// include global test defs
#include "cpe_test_global.hh"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "soap/Auth.hh"

using namespace tr064;
using namespace tr064::soap;
using namespace ::testing;

namespace 
{
  static const std::string USERNAME = "foo";
  static const std::string PASSWORD = "bar";
  const char* NONCE_1 = "9AA2EE28C70467A2";
  const char* NONCE_2 = "38CDA2685E285E51";
  const char* REALM = "L!bTr064 SOAP-Auth";
  const char* AUTH_RESPONSE_1 = "24aeacdf3a10f96fbc5d08fa5260354a";
  const char* AUTH_RESPONSE_2 = "853ef3598049ac47dc7403b20206f7cd";
  
  //----------------------------------------------------------------------------

  bool 
  has_xml_node_with_value(const pugi::xml_node& node, 
      const std::string& desired_node_name,
      const std::string& desired_node_value = "")
  {
    auto desired_node = 
      node.find_node(
          [desired_node_name, desired_node_value](pugi::xml_node current_node)
          {
            //std::cerr << "desired name: " << desired_node_name << " desired value: " << desired_node_value << std::endl;
            //std::cerr << "current_node name: " << current_node.name() << " current_node value: " << current_node.child_value() << std::endl;
            bool r = current_node.name() == desired_node_name; 
            if ( !desired_node_value.empty() )
              r &= current_node.child_value() == desired_node_value;
            //std::cerr << "result: " << std::boolalpha << r << std::endl;
            return r;
          });
    return !desired_node.empty();
  }

  //----------------------------------------------------------------------------

  bool 
  has_xml_node_with_value(
      const std::string& node, 
      const std::string& desired_node_name,
      const std::string& desired_node_value = "")
  {
    pugi::xml_document doc;
    doc.load(node.c_str());
    return has_xml_node_with_value(doc, desired_node_name, desired_node_value);
  }
}

//------------------------------------------------------------------------------

TEST(SoapAuthTest, AuthOk)
{

  Auth auth(USERNAME, PASSWORD);

  EXPECT_FALSE( auth.is_authenticated() );
  EXPECT_EQ( Auth::IDLE, auth.get_current_state() );

  auto auth_header = auth.get_auth_header_for_request();

  pugi::xml_document doc;
  doc.load(auth_header.c_str());

  EXPECT_TRUE( has_xml_node_with_value(doc, "h:InitChallenge") );
  EXPECT_TRUE( has_xml_node_with_value(doc, "UserID", USERNAME) );
  EXPECT_EQ( Auth::INIT_CHALLANGE, auth.get_current_state() );

  pugi::xml_document init_resp_doc;
  auto n_envelope = init_resp_doc.append_child("s:Envelope");
  auto n_header = n_envelope.append_child("s:Header");
  auto n_h_challenge = n_header.append_child("h:Challenge");
  n_h_challenge.append_child("Status").text().set("Unauthenticated");
  n_h_challenge.append_child("Nonce").text().set(NONCE_1);
  n_h_challenge.append_child("Realm").text().set(REALM);

  auto n_body = n_envelope.append_child("s:Body");
  auto n_b_fault = n_body.append_child("s:Fault");

  n_b_fault.append_child("faultcode").text().set("s:Client");
  n_b_fault.append_child("faultstring").text().set("UPnPError");

  auto n_b_detail = n_b_fault.append_child("detail");
  auto n_err = n_b_detail.append_child("UPnPError");
  n_err.append_child("errorCode").text().set("503");
  n_err.append_child("errorDescription").text().set("Auth. failed");

  auth.handle_auth_response(n_envelope);
  EXPECT_FALSE( auth.is_authenticated() );
  EXPECT_EQ( Auth::INIT_CHALLANGE_RESPONSE_HANDLED, auth.get_current_state() );

  auth_header = auth.get_auth_header_for_request();
  doc.reset();
  doc.load(auth_header.c_str());

  EXPECT_TRUE( has_xml_node_with_value(doc, "h:ClientAuth") );
  EXPECT_TRUE( has_xml_node_with_value(doc, "UserID", USERNAME) );
  EXPECT_TRUE( has_xml_node_with_value(doc, "Nonce", NONCE_1) );
  EXPECT_TRUE( has_xml_node_with_value(doc, "Auth", AUTH_RESPONSE_1 ) );
  EXPECT_EQ( Auth::INIT_CHALLANGE_RESPONSE_HANDLED, auth.get_current_state() );
  EXPECT_FALSE( auth.is_authenticated() );

  init_resp_doc.reset();
  n_envelope = init_resp_doc.append_child("s:Envelope");
  n_header = n_envelope.append_child("s:Header");
  n_h_challenge = n_header.append_child("h:NextChallenge");
  n_h_challenge.append_child("Status").text().set("Authenticated");
  n_h_challenge.append_child("Nonce").text().set(NONCE_2);
  n_h_challenge.append_child("Realm").text().set(REALM);

  auth.handle_auth_response(n_envelope);

  auth_header = auth.get_auth_header_for_request();
  doc.reset();
  doc.load(auth_header.c_str());

  EXPECT_TRUE( has_xml_node_with_value(doc, "h:ClientAuth") );
  EXPECT_TRUE( has_xml_node_with_value(doc, "UserID", USERNAME) );
  EXPECT_TRUE( has_xml_node_with_value(doc, "Nonce", NONCE_2) );
  EXPECT_TRUE( has_xml_node_with_value(doc, "Auth", AUTH_RESPONSE_2 ) );
  EXPECT_EQ( Auth::AUTHENTICATED, auth.get_current_state() );
  EXPECT_TRUE( auth.is_authenticated() );
}

//------------------------------------------------------------------------------
