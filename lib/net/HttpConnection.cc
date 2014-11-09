#include "HttpConnection.hh"

#include <tr064/trace.hh>

#include "../Tr064Exception.hh"

// boost
#include <boost/asio.hpp>
#include <boost/algorithm/string/predicate.hpp>

// stl
#include <sstream>

using namespace tr064;
using namespace boost::asio::ip;

namespace 
{
  void remove_tailering_cr(std::string& str)
  {
    auto pos = str.rfind('\r');
    if ( pos != std::string::npos )
      str = str.substr(0, pos);
  }

}

//------------------------------------------------------------------------------

void
HttpConnection::connect()
{
  if ( _host.empty() || _port.empty() )
    throw Tr064Exception("Host and Port can not be empty");

  if ( _socket.is_open() )
  {
    LOG_DEBUG("Socket is already open, will close it.")
    _socket.close();
  }
  LOG_TRACE("Try to open a socket to: " << _host << " on port: " << _port);

  tcp::resolver resolver(_io_service);
  tcp::resolver::query query(_host, _port);
  tcp::resolver::iterator endpoint_it = resolver.resolve(query);
  tcp::resolver::iterator end_it;

  boost::system::error_code error = boost::asio::error::host_not_found;
  unsigned attempts = 0;
  while ( error && endpoint_it != end_it )
  {
    ++attempts;
    //_socket.close();
    _socket.connect(*endpoint_it, error);
    ++endpoint_it;
  }
  LOG_DEBUG("Connect attempts: " << attempts << 
        " connected to: " << _socket.remote_endpoint().address().to_string() 
        << ":" << _socket.remote_endpoint().port());

  if (error && error != boost::asio::error::eof)
    throw Tr064Exception("Error occured during connect: " + error.message());

  if ( !_socket.is_open() )
    throw Tr064Exception("Socket is not open, unable to continue!");
}

//------------------------------------------------------------------------------

HttpResponse*
HttpConnection::sync(const HttpRequest* request)
{
  connect();
                                 
  HeaderEntry header;
  HeaderList header_list;
  std::streambuf* body_buf = new std::stringbuf();

  {// write to open socket
    auto request_buf = request->get_buffer();
    boost::asio::write( _socket, *request_buf);
    LOG_TRACE("Request send.");
  }
  { // handle HTTP headers
    boost::asio::streambuf response_buf;
    std::istream response_stream(&response_buf);

    // first the status line
    // HTTP/1.1 200 OK

    auto size_read = boost::asio::read_until(_socket, response_buf, "\r\n");
    LOG_DEBUG("1. read for read_untl \"\\r\\n\": " << size_read);

    std::string http_version;
    unsigned status_code;
    std::string status_message;

    response_stream >> http_version;
    response_stream >> status_code;
    response_stream >> status_message;              

    std::string m = 
          "code: " + std::to_string(status_code) +
          " version: " + http_version + 
          " message: " + status_message;
    if (!response_stream || http_version.substr(0, 5) != "HTTP/")
    {
      throw Tr064Exception("Invalid HTTP response: " + m);
    }     
    if ( status_code != 200 )
    {
      throw Tr064Exception("Invalid HTTP response code: " + m);
    }
    LOG_DEBUG(m);
    LOG_DEBUG("Left in response_buf: " << response_buf.size());

    size_read = boost::asio::read_until(_socket, response_buf, "\r\n\r\n");

    LOG_DEBUG("2. read for read_untl \"\\r\\n\\r\\n\": " << size_read);

    LOG_DEBUG("Left in response_buf: " << response_buf.size());

    // Read the response headers, which are terminated by two CR LF
    while (std::getline(response_stream, header) && header != "\r")
    {
      remove_tailering_cr(header);
      header_list.push_back(header);
      LOG_TRACE("Header line read: " + header);
    }
                       
    boost::system::error_code error;
    std::ostream body_stream(body_buf);
    while ( (size_read = boost::asio::read(_socket, response_buf, 
          boost::asio::transfer_at_least(1), error)) ) 
    {
      LOG_DEBUG("3. read until end: " << size_read );
      body_stream << &response_buf;
    }

    if (error && error != boost::asio::error::eof)
      throw Tr064Exception("Error occured during connect: " + error.message());
    // TODO we could close the socket here.
    _socket.close();

    LOG_DEBUG("Body done!");
  }

  return new HttpResponse(header_list, body_buf);
}
