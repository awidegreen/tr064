#ifndef HTTPREQ_HH
#define HTTPREQ_HH

#include "HttpResponse.hh"

#include <boost/asio/streambuf.hpp>

#include <ostream>
#include <string>

namespace tr064
{

class HttpRequest 
{
public:
  HttpRequest(const std::ostream& headers, const std::ostream& body ) :
    _headers(headers),
    _body(body)
  { }

  // make inheritable
  virtual ~HttpRequest() { }

  boost::asio::streambuf* get_buffer() const;

  //void exec_async(void* [> there should be callback class here<]) const;
private:
  const std::ostream& _headers;
  const std::ostream& _body;
};

}

#endif /* HTTPREQ_HH */
