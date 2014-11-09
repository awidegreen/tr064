#include "HttpRequest.hh"
#include "HttpCommon.hh"

#include <tr064/trace.hh> 

// boost 
#include <boost/asio.hpp>

// std lib
#include <exception>
#include <iostream>

using namespace tr064;

boost::asio::streambuf*
HttpRequest::get_buffer() const
{
  auto request_buf = new boost::asio::streambuf();
  std::ostream req_stream(request_buf);
  req_stream << _headers.rdbuf();
  req_stream << "\r\n\r\n";
  if ( _body.rdbuf()->in_avail() )
    req_stream << _body.rdbuf(); 

  return request_buf;
}

//------------------------------------------------------------------------------

//HttpResponse*
//HttpRequest::sync(const std::ostream& headers, const std::ostream& body) const 
//{
  //if ( _host.empty() || _port.empty() )
  //{
    //std::cerr << "Host and Port can not be empty" << std::endl;
    //throw std::exception();
  //}

  ////std::cerr << "host: " << _host << " port: " << _port << std::endl; 

  //using namespace boost::asio::ip;

  //boost::asio::io_service io_service;
                                  
  //tcp::resolver resolver(io_service);
  //tcp::resolver::query query(_host, _port);
  //tcp::resolver::iterator endpoint_it = resolver.resolve(query);
  //tcp::resolver::iterator end_it;

  //// Try each endpoint until we successfully establish a connection.
  //tcp::socket socket(io_service);
  //boost::system::error_code error = boost::asio::error::host_not_found;
  //while (error && endpoint_it != end_it)
  //{
    //socket.close();
    //socket.connect(*endpoint_it++, error);
  //}

  //if (error)
    //throw boost::system::system_error(error);

  //// Form the request. We specify the "Connection: close" header so that the
  //// server will close the socket after transmitting the response. This will
  //// allow us to treat all data up until the EOF as the content.
  //boost::asio::streambuf request;
  //std::ostream req_stream(&request);
  //req_stream << headers.rdbuf();
  //req_stream << "\r\n\r\n";
  //if ( body.rdbuf()->in_avail() )
    //req_stream << body.rdbuf(); 
   
  //// Send the request.
  //boost::asio::write(socket, request);

  //// Read the response status line.
  //boost::asio::streambuf response;
  //boost::asio::read_until(socket, response, "\r\n");

  //// Check that response is OK.
  //// response will look like:
  //// HTTP/1.1 200 OK
  //// [other headers]
  //std::istream response_stream(&response);
  //std::string http_version;
  //response_stream >> http_version;
  //unsigned int status_code;
  //response_stream >> status_code;
  //std::string status_message;
  //std::getline(response_stream, status_message);
  //if (!response_stream || http_version.substr(0, 5) != "HTTP/")
  //{
    //std::cerr << "Invalid response\n";
    //throw std::exception();
  //}
  //if (status_code != 200)
  //{
    //std::cerr << "Response returned with status code " << status_code << "\n";
    //return new HttpResponse(status_code);
  //}

  //// Read the response headers, which are terminated by a blank line.
  //boost::asio::read_until(socket, response, "\r\n\r\n");

  //// Process the response headers.
  //HeaderEntry header;
  //HeaderList header_list;
  //while (std::getline(response_stream, header) && header != "\r")
  //{
    //// store headers in list
    //header_list.push_back(header);
  //}

  //response.consume(response.size());

  //auto body_response = new boost::asio::streambuf();
  //std::ostream body_stream(body_response);

  //// Read until EOF, writing data to output as we go.
  //while (boost::asio::read(socket, response,
        //boost::asio::transfer_at_least(1), error))
  //{
    //body_stream << &response;
  //}

  //if (error != boost::asio::error::eof)
    //throw boost::system::system_error(error);
  
  //auto resp = new HttpResponse(header_list, body_response);

  //return resp;
//}

//------------------------------------------------------------------------------

//HttpResponse*
//sync2(const std::ostream& headers, const std::ostream& body) const 
//{

  ////std::cerr << "host: " << _host << " port: " << _port << std::endl; 

  //using namespace boost::asio::ip;

  //boost::asio::io_service io_service;
                                  
  //tcp::resolver resolver(io_service);
  //tcp::resolver::query query(_host, _port);
  //tcp::resolver::iterator endpoint_it = resolver.resolve(query);
  //tcp::resolver::iterator end_it;

  //// Try each endpoint until we successfully establish a connection.
  //tcp::socket socket(io_service);
  //boost::system::error_code error = boost::asio::error::host_not_found;
  //while (error && endpoint_it != end_it)
  //{
    //socket.close();
    //socket.connect(*endpoint_it++, error);
  //}

  //if (error)
    //throw boost::system::system_error(error);

  //// Form the request. We specify the "Connection: close" header so that the
  //// server will close the socket after transmitting the response. This will
  //// allow us to treat all data up until the EOF as the content.
  //boost::asio::streambuf request;
  //std::ostream req_stream(&request);
  //req_stream << headers.rdbuf();
  //req_stream << "\r\n\r\n";
  //if ( body.rdbuf()->in_avail() )
    //req_stream << body.rdbuf(); 
   
  //// Send the request.
  //boost::asio::write(socket, request);

  //// Read the response status line.
  //boost::asio::streambuf response;

  //auto response_buf = new boost::asio::streambuf();
  //std::ostream response_stream(response_buf);
  ////while (boost::asio::read(socket, response,
         ////boost::asio::transfer_at_least(0),resp_error))
  //while (auto read = boost::asio::read(socket, response,
         //boost::asio::transfer_all(), error))
  //{
    ////ss << &response;
    //response_stream << &response;
    //TRACE("Amount of data read: " << read << " error: " << error);
  //}
  //TRACE("done with the body stream");

  //if (error && error != boost::asio::error::eof)
    //throw boost::system::system_error(error);


  //HeaderList header_list;
  //auto body_buf = new boost::asio::streambuf();
  //std::ostream body_stream(body_buf);

  //std::istream response_reader(response_buf);

  //std::string line; 
  //while ( std::getline(response_reader, line) )
  //{
    //bool is_header = false;
    //auto pos = line.find('\r');
    //if ( pos != std::string::npos )
    //{
      //line = line.substr(0,pos);
      //is_header = true;
    //}
    //// DEBUG
    //TRACE("Current line size: " << line.size() << " val: '" << line << "'");

    //if ( is_header )
    //{ 
      //// TODO maybe check for the first HTTP header here and response code!
      //header_list.push_back(line);
      //continue;
    //}

    //// this is the body 
    //if ( line.empty() )
      //continue; // that seems to be to blank line between headers and body.

    //body_stream << line << '\n';
  //}
  //TRACE("done with constructing response");

  //auto resp = new HttpResponse(header_list, body_buf);

  //return resp;
//}
