#include "Auth.hh"

#include <tr064/trace.hh>

#include <pugixml/pugixml.hpp>
#include <openssl/md5.h>

#include "../Tr064Exception.hh"

// stl
#include <iostream>
#include <sstream>

using namespace tr064::soap;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
namespace 
{

//------------------------------------------------------------------------------
  std::string md5(const std::string& input)
  {
    unsigned char digest[MD5_DIGEST_LENGTH];

    MD5((unsigned char*)input.c_str(), input.size(), (unsigned char*)&digest);    

    char mdString[33];

    for(int i = 0; i < 16; i++)
      sprintf(&mdString[i*2], "%02x", (unsigned int)digest[i]);

    return mdString;
  }

  //----------------------------------------------------------------------------

  std::string generate_response_challange(
      const std::string& userid,
      const std::string& realm,
      const std::string& password,
      const std::string& sn /* aka nonce */
      )
  {
    auto secret = md5( userid + ":" + realm + ":" + password );
    auto response = md5(secret + ":" + sn);
    return response;
  }
  
  //----------------------------------------------------------------------------

  const pugi::xml_node find_node_by_name(const pugi::xml_node& node, 
                                          const std::string& search_node_name) 
  {
    return node.find_node([search_node_name](pugi::xml_node current_node)
        {
          return current_node.name() == search_node_name;
        });
  }

  //----------------------------------------------------------------------------

  struct SoapError
  {
    int code;
    std::string msg;
  };

  SoapError* 
  has_error(const pugi::xml_node& body_node)
  {
    const auto& fault_node = body_node.child("s:Fault");
    if ( fault_node.empty() )
      return nullptr;
    LOG_DEBUG("Found s:Fault node.");


    auto node_err_code = find_node_by_name(body_node, "errorCode");
    auto node_err_msg =  find_node_by_name(body_node, "errorDescription");

    if ( !node_err_code.empty() && !node_err_msg.empty() )
    {
      try
      {
        auto code = std::stoi(node_err_code .child_value());
        return new SoapError { code, node_err_msg.child_value() };
      }
      catch (std::exception e)
      {
        std::cerr << "Unable to convert error code to number." << std::endl;
      }
    }

    return new SoapError{0, ""};
  }
}
//------------------------------------------------------------------------------

std::string
Auth::state2str(Auth::State state)
{
  switch ( state )
  {
    case IDLE : return "IDLE";
    case INIT_CHALLANGE : return "INIT_CHALLANGE";
    case INIT_CHALLANGE_RESPONSE_HANDLED: return "INIT_CHALLANGE_RESPONSE_HANDLED";
    case AUTHENTICATED : return "AUTHENTICATED";
  }

}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

std::string
Auth::get_auth_header_for_request()
{
  std::stringstream ss;
  LOG_TRACE("Current state: " << state2str(_state));

  switch ( _state )
  {
    case IDLE : 
    {
      // do initial challange
      ss 
        << "<h:InitChallenge xmlns:h=\"http://soap-authentication.org/digest/2001/10/\" s:mustUnderstand=\"1\">" << std::endl
        << "  <UserID>" << _user << "</UserID>"  << std::endl
        << "</h:InitChallenge>" << std::endl;
      _state = INIT_CHALLANGE;
      break;
    }
    case INIT_CHALLANGE_RESPONSE_HANDLED:
    case AUTHENTICATED : 
    {
      ss
        << "<h:ClientAuth xmlns:h=\"http://soap-authentication.org/digest/2001/10/\" s:mustUnderstand=\"1\">" << std::endl
        << "  <Nonce>"  << _nonce << "</Nonce>" << std::endl
        << "  <Auth>"   << generate_response_challange(_user, _realm, _password, _nonce) << "</Auth>" << std::endl
        << "  <UserID>" << _user << "</UserID>" << std::endl
        << "  <Realm>"  << _realm << "</Realm>" << std::endl
        << "</h:ClientAuth>" << std::endl;
      break;
    }
    default:
      throw tr064::Tr064Exception(
          "[Auth:get_auth_header_for_request] Unable to proceed in State: " + 
          state2str(_state));
  }

  //std::cerr << "use the auth headers: " << std::endl << ss.str() << std::endl;

  return ss.str();
}

//------------------------------------------------------------------------------


void
Auth::handle_auth_response(const pugi::xml_node& envelope_node)
{
  LOG_TRACE("Current state: " << state2str(_state));
  std::stringstream ss;
  LOG_DEBUG("current envelope_node: " << envelope_node.name() << " children: ");
  
  const auto& header_node = envelope_node.child("s:Header");
  const auto& body_node   = envelope_node.child("s:Body");
  std::string challenge_node_name;
  State next_state = IDLE;

  switch ( _state )
  {
    case INIT_CHALLANGE : 
    {
      if ( !has_error(body_node) )
      {
        throw tr064::Tr064Exception("The Authentication should have failed for "
            "the initial SOAP auth. challenge.");
      }
      challenge_node_name = "h:Challenge";
      next_state = INIT_CHALLANGE_RESPONSE_HANDLED;
      break;
    }
    case INIT_CHALLANGE_RESPONSE_HANDLED:
    {
      auto error = has_error(body_node);
      if ( error )
      {
        throw tr064::Tr064Exception(
            "SOAP authentication failed, password incorrect?\n"
            "  Code: " + std::to_string(error->code) + 
            "\n  Description " + error->msg );
      }
      challenge_node_name = "h:NextChallenge";
      next_state = AUTHENTICATED;
      break;
    }
    case AUTHENTICATED : 
    {
      auto error = has_error(body_node);
      if ( error )
      {
        throw tr064::Tr064Exception(
            "SOAP authentication failed, password incorrect?\n"
            "  Code: " + std::to_string(error->code) + 
            "\n  Description " + error->msg );
      }
      challenge_node_name = "h:NextChallenge";
      break;
    }
    default:
      throw tr064::Tr064Exception(
          "[Auth:handle_auth_response] Unable to proceed in State: " + 
          state2str(_state));
      break;
  }

  //header_node.text();

  const auto& challenge_node = header_node.child(challenge_node_name.c_str());
  if ( !challenge_node.empty() )
  {
    update_nonce_realm(challenge_node);
    _state = next_state;
  }
  else 
    throw tr064::Tr064Exception(
        "Unable to find " + challenge_node_name + 
        " node in envelope_node " + state2str(_state));
}

//------------------------------------------------------------------------------

void 
Auth::update_nonce_realm(const pugi::xml_node& node)
{
  auto status = node.child("Status");
  if ( !status.empty() )
  {
    //std::cout << "status value: " << status.child_value() << std::endl;
    auto nonce = node.child("Nonce");
    if ( !nonce.empty() )
    {
      _nonce = nonce.child_value();
      auto realm = node.child("Realm");
      if ( !realm.empty() )
      {
        _realm = realm.child_value();
      }
      else 
      {
        throw tr064::Tr064Exception("Unable to find Realm node!");
      }
    }
    else 
      throw tr064::Tr064Exception("Unable to find Nonce node!");
  }
  else 
    throw tr064::Tr064Exception("Unable to find Status node!");
}
