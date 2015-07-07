#ifndef HTTPREQ_HH
#define HTTPREQ_HH

#include "HttpResponse.hh"
#include "HttpCommon.hh"

#include <vector>
#include <string>

namespace tr064
{

class HttpRequest 
{
public:
  enum RequestType { GET, POST };

  HttpRequest(
      RequestType type,
      const std::string& url,
      const HeaderList& headers,
      const std::string& body ) :
    _type(type),
    _url(url),
    _headers(headers),
    _body(body)
  { }

  const std::string& url() const { return _url; }
  const HeaderList& headers() const { return _headers; }
  const RequestType& type() const { return _type; }
  const std::string& body() const { return _body; }

private:
  RequestType _type;
  std::string _url;
  const HeaderList& _headers;
  const std::string& _body;
};

}

#endif /* HTTPREQ_HH */
