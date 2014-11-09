#ifndef HTTPRESPONSE_HH
#define HTTPRESPONSE_HH

#include "HttpCommon.hh"

#include <streambuf>

namespace tr064
{

class HttpResponse
{
public:
  HttpResponse(unsigned statuscode) :
    _statuscode(statuscode),
    _body(nullptr) 
  { }
  
  HttpResponse(const HeaderList& headers,
               std::streambuf* body) :
    _statuscode(200),
    _headers(headers),
    _body(body)
  { }

  // make inheritable
  //virtual ~HttpResponse() { if ( _body ) delete _body; }

  bool empty() const  { return _headers.empty() && _body == nullptr; }

  unsigned status_code() const { return _statuscode; }
  const HeaderList& headers() const { return _headers; }
  std::streambuf* body() { return _body; }

private:
  unsigned int _statuscode;
  HeaderList _headers;
  std::streambuf* _body;
};

}

#endif /* HTTPRESPONSE_HH */
