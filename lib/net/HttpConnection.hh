#ifndef HTTPCONNECTION_HH
#define HTTPCONNECTION_HH

#include "HttpResponse.hh"
#include "HttpRequest.hh"

// boost
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

// stl
#include <string>

namespace tr064
{

class HttpConnection
{
public:
  HttpConnection(const std::string& host, const std::string& port) :
    _host(host),
    _port(port),
    _socket(_io_service)
  { 
  }

  ~HttpConnection() 
  {
    _socket.close();
  }

  HttpResponse* sync(const HttpRequest* request);

private:
  void connect();

  std::string _host;
  std::string _port;
  boost::asio::io_service _io_service;
  boost::asio::ip::tcp::socket _socket;
};

} // ns

#endif /* HTTPCONNECTION_HH */
