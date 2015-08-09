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
  struct Credentials
  { 
    std::string username; 
    std::string password; 
    bool empty() const { return username.empty(); }
  };

  virtual ~HttpConnection() { }

  virtual HttpResponse* sync(const HttpRequest* request) = 0;

  void set_credentials(const Credentials& c) { _credentials = c; }


protected:
  HttpConnection() { }
  Credentials _credentials;
};

}


#endif /* HTTPCONNECTION_HH */
