#ifndef HTTPCONNECTION_HH
#define HTTPCONNECTION_HH

#include <string>

namespace tr064
{
class HttpRequest;
class HttpResponse;

class HttpConnection
{
public:
  virtual ~HttpConnection() { }

  virtual HttpResponse* sync(const HttpRequest* request) = 0;


protected:
  HttpConnection() { }

};

}


#endif /* HTTPCONNECTION_HH */
