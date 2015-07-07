#include "Session.hh"

#include "../net/HttpConnectionCurl.hh"
#include "../net/HttpRequest.hh"
#include "../net/HttpCommon.hh"
#include <tr064/trace.hh>

#include "../Tr064Exception.hh"

#include <pugixml/pugixml.hpp>

#include <sstream>
#include <iostream>
#include <memory>

using namespace tr064::soap;

namespace 
{
  tr064::HeaderList
  get_headers(
      const tr064::Service::Ptr& service,        
      const tr064::ServiceAction::Ptr& action)
  {
    std::stringstream ss;
    
    tr064::HeaderList headers;

    headers.push_back("Content-Type: text/xml; charset=\"utf-8\"");
    headers.push_back("Soapaction: \"" + service->type() + "#" + action->name() + "\"");
    headers.push_back("User-Agent: upnp/1.0 client 1.0 - awidegreen/tr064lib");

    return headers;
  }

}

//------------------------------------------------------------------------------

pugi::xml_node
Session::execute_action(
      const Service::Ptr& service, 
      const ServiceAction::Ptr& action) 
{

  LOG_TRACE("Going to execute Service: " + service->name() + 
            " with action: " + action->name());

  std::unique_ptr<HttpConnection> http_conn(new HttpConnectionCurl());

  pugi::xml_document doc;
  // iterate as long as it is not authenticated
  do 
  {
    // reset xml docuemnt.
    doc.reset();

    auto body_str = get_body(service, action);
    auto headers = get_headers(service, action);

    LOG_DEBUG("BODY_STR: " + body_str);

    std::string url = "http://" + _host + ":" + _port + service->control_url();
    HttpRequest req(HttpRequest::POST, url, headers, body_str);
    auto response = http_conn->sync(&req);

    std::ostringstream r_oss;
    r_oss << (response->body());
    std::string r_s = r_oss.str();

    doc.load(r_s.c_str());

    // used for debug
    std::stringstream d_ss; doc.save(d_ss); LOG_DEBUG(d_ss.str());

    const auto& envelope_node = doc.child("s:Envelope");
    if ( envelope_node.empty() )
    {
      throw tr064::Tr064Exception(
          "Received response does not contain an envelope_node, "
          "unable to proceed, received: \n" + r_s);
    }

    if ( !_no_auth )
      _auth.handle_auth_response(envelope_node);

    delete response;

  } while ( !_auth.is_authenticated() && !_no_auth );

  // the xml doc should be filled at this point
  auto body_node = doc.child("s:Envelope").child("s:Body");
  if ( body_node.empty() )
  {
    std::cerr << "Received body node is empty." << std::endl;
  }
  return body_node;
}

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------

std::string
Session::get_body(
      const Service::Ptr& service,
      const ServiceAction::Ptr& action)
{
  std::stringstream ss;

  ss << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << std::endl
     << "<s:Envelope s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\" xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\">" << std::endl;

  if (!_no_auth)
  {
     ss << "  <s:Header>" << std::endl
        << _auth.get_auth_header_for_request() 
        << "  </s:Header>" << std::endl;
  }
  ss << "  <s:Body>" << std::endl 
     << "    <u:"  << action->name() << " xmlns:u=\"" << service->type() << "\">" << std::endl;

  for ( const auto& in_arg : action->args() )
  {
    // skip out args
    if ( in_arg->direction() == ServiceActionArg::OUT ) continue;
    // skip empty in args
    if ( in_arg->value().empty() ) continue;

    ss << "       <" << in_arg->name() << ">" 
       << in_arg->value() 
       << "       </" << in_arg->name() << ">" << std::endl;
  }

  ss << "    </u:" << action->name() << ">" << std::endl
     << "  </s:Body>" << std::endl
     << "</s:Envelope>" << std::endl;

  return ss.str();
}
