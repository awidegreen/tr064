#include "Session.hh"

#include "../net/HttpConnection.hh"
#include "../net/HttpRequest.hh"
#include <tr064/trace.hh>

#include "../Tr064Exception.hh"

#include <pugixml/pugixml.hpp>

#include <sstream>
#include <iostream>

using namespace tr064::soap;


//------------------------------------------------------------------------------

pugi::xml_node
Session::execute_action(
      const Service::Ptr& service, 
      const ServiceAction::Ptr& action) 
{

  LOG_TRACE("Going to execute Service: " + service->name() + 
            " with action: " + action->name());

  HttpConnection http_conn(_host, _port);

  pugi::xml_document doc;
  // iterate as long as it is not authenticated
  do 
  {
    // reset xml docuemnt.
    doc.reset();

    auto body_str = get_body(service, action);
    std::stringstream ss;
    //ss << headers
       //<< body;

    std::stringbuf headers_buf;
    std::ostream headers(&headers_buf);
    auto header_str = get_headers(service, action, body_str.size()+4);
    headers << header_str;

    std::stringbuf body_buf;
    std::ostream body(&body_buf);
    body << body_str;

    LOG_DEBUG("Current request: " << std::endl << header_str << body_str);

    HttpRequest req(headers, body);
    auto response = http_conn.sync(&req);
    //return "";

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

std::string
Session::get_headers(
    const Service::Ptr& service,        
    const ServiceAction::Ptr& action,
    const size_t body_length)
{
  std::stringstream ss;

  ss << "POST "  << service->control_url() << " HTTP/1.1\r\n" 
     << "HOST: " << _host << "\r\n" 
     << "CONTENT-LENGTH: " << body_length << "\r\n" 
     << "CONTENT-TYPE: text/xml; charset=\"utf-8\"" << "\r\n" 
     << "SOAPACTION: \"" << service->type() << "#"  << action->name() << "\"" << "\r\n" 
     << "CONNECTION: close\r\n"
     << "USER-AGENT: AVM UPnP/1.0 Client 1.0 - tr064 lib" << "\r\n\r\n";

  return ss.str();
}

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
