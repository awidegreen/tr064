#include <tr064/ResourceRetrieval.hh>

#include "net/HttpRequest.hh"
#include "net/HttpConnection.hh"

#include "Tr064Exception.hh"


// std lib
#include <string>
#include <istream>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace tr064;

//------------------------------------------------------------------------------
// CLASS ResourceRetreivalHttp
//------------------------------------------------------------------------------

ResourceRetreivalHttp::ResourceRetreivalHttp(const std::string& host,
                                             const std::string& port) :
  _host(host),
  _port(port)
{ }

//------------------------------------------------------------------------------

ResourceStream
ResourceRetreivalHttp::get(const std::string& path) const
{
  if ( _host.empty() || path.empty() )
  {
    throw Tr064Exception("Hostname and Path can not be empty!");
  }

  std::cout << "Fetch resource (GET) http://" 
            << _host << ":" << _port << path << std::endl;

  //std::clog << "Using host: " << _host 
            //<< " and port: " << _port 
            //<< " path: "<< path << std::endl;
                                  

  // Form the request. We specify the "Connection: close" header so that the
  // server will close the socket after transmitting the response. This will
  // allow us to treat all data up until the EOF as the content.
  std::stringbuf headers_buf;
  std::ostream headers(&headers_buf);
  headers << "GET " << path << " HTTP/1.0\r\n";
  headers << "Host: " << _host << ":" << _port << "\r\n";
  headers << "Accept: */*\r\n";
  headers << "Connection: close\r\n\r\n";

  std::stringbuf body_buf;
  std::ostream body(&body_buf);


  HttpConnection http_conn(_host, _port);
  HttpRequest req(headers, body);
  auto response = http_conn.sync(&req);

  auto os = new std::istream(response->body());
  ResourceStream result_stream(os);
  
  delete response;
  
  return result_stream;
}

//------------------------------------------------------------------------------
// CLASS ResourceRetreivalFile
//------------------------------------------------------------------------------

ResourceStream
ResourceRetreivalFile::get(const std::string& path) const
{
  std::ifstream file(path);
  
  if ( !file.is_open() )
  {
    // if file can not be open, bail out
    throw Tr064Exception("Can not open resource file: " + path);
  }

  auto ss = new std::stringstream;

  std::string line;
  while ( std::getline(file, line))
  {
    *ss << line;
  }

  ResourceStream result_stream(ss);

  return result_stream;

}

//------------------------------------------------------------------------------
// CLASS ResourceRetreivalEmpty
//------------------------------------------------------------------------------

ResourceStream
ResourceRetreivalEmpty::get(const std::string& /*path*/) const
{

  ResourceStream result_stream(new std::stringstream());

  return result_stream;

}
