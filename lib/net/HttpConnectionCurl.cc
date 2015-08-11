#include "HttpConnectionCurl.hh"
#include "HttpResponse.hh"
#include "HttpRequest.hh"

#include "../Tr064Exception.hh"
#include <tr064/trace.hh>

// curl
#include <curl/curl.h>

// curlcpp 
#include <curl_header.h>

// stl
#include <iostream>
#include <sstream>
#include <string>

using namespace tr064;
using curl::curl_easy;
using curl::curl_header;

//------------------------------------------------------------------------------

HttpConnectionCurl::HttpConnectionCurl() 
{ 
  _cwriter = curl_writer(_out_stream);
  _ceasy = std::unique_ptr<curl::curl_easy>(new curl::curl_easy(_cwriter));
  // keep tcp alive
  _ceasy->add(curl_pair<CURLoption, long>(CURLOPT_TCP_KEEPALIVE, 1L));
}

//------------------------------------------------------------------------------

HttpResponse*
HttpConnectionCurl::sync(const HttpRequest* request)
{

  LOG_DEBUG("HTTP request to: " + request->url());

  curl_header c_headers;
  if ( !request->headers().empty() )
  {
    for ( const auto& hdr : request->headers() ) 
    {
      LOG_DEBUG("HEADER to curl: " + hdr);
      c_headers.add(hdr);
    }

    _ceasy->add(
        curl_pair<CURLoption, curl_header>(CURLOPT_HTTPHEADER, c_headers));
  }

  _ceasy->add(curl_pair<CURLoption, std::string>(CURLOPT_URL, request->url()));
  _ceasy->add(curl_pair<CURLoption, long>(CURLOPT_HEADER, 1L));
  //_ceasy->add(curl_pair<CURLoption, long>(CURLOPT_VERBOSE, 1L));

  if ( request->type() == HttpRequest::POST )
  {
    _ceasy->add(curl_pair<CURLoption, long>(CURLOPT_POST, 1L));
    _ceasy->add(curl_pair<CURLoption, std::string>(CURLOPT_POSTFIELDS, 
          request->body()));
    //_ceasy->add(curl_pair<CURLoption, long>(CURLOPT_POSTFIELDSIZE, 
          //request->body().size()));
  }


  if ( !_credentials.empty() )
  { 
    _ceasy->add(curl_pair<CURLoption, std::string>(CURLOPT_USERPWD, 
          _credentials.username + ":" + _credentials.password));
    _ceasy->add(curl_pair<CURLoption, long>(CURLOPT_HTTPAUTH, CURLAUTH_ANY));
  }

  try 
  {
    _ceasy->perform();
  }
  catch (curl_easy_exception& error)
  { 
    error.print_traceback(); 
    std::stringstream ssdesc;
    ssdesc << "Curl problem: " << error.what() << " " << error.get_code();
    throw Tr064Exception(ssdesc.str());
    return nullptr;
  }

  std::string http_version;
  unsigned status_code;
  std::string status_message;

  _out_stream >> http_version;
  _out_stream >> status_code;
  _out_stream >> status_message;

  HeaderEntry header;
  HeaderList header_list;
  // Read the response headers, which are terminated by two CR LF
  while (std::getline(_out_stream, header) && header != "\r")
  {
    remove_tailering_cr(header);
    header_list.push_back(header);
    LOG_DEBUG("Header line read: " + header);
  }

  std::streambuf* body_buf = new std::stringbuf();
  std::iostream body_stream(body_buf);
  body_stream << _out_stream.str();

  // empty the curl stream
  _out_stream.str(std::string());

  return new HttpResponse(header_list, body_buf);
}
