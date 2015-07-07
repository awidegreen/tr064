#ifndef CURLHTTPCONNECTION_HH
#define CURLHTTPCONNECTION_HH

#include "HttpConnection.hh"

#include <curl_easy.h>
#include <curl_writer.h>

// stl
#include <sstream>
#include <memory>
#include <ostream>


namespace tr064
{

class HttpConnectionCurl : public HttpConnection
{
public:
  HttpConnectionCurl();

  virtual HttpResponse* sync(const HttpRequest* request);

private:
  std::stringstream _out_stream;
  curl::curl_writer _cwriter;
  std::unique_ptr<curl::curl_easy> _ceasy;
};

}


#endif /* CURLHTTPCONNECTION_HH */
