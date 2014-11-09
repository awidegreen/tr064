#include "Utils.hh"


std::string 
tr064::extract_type_name(const std::string& full_type_name)
{
  // full type name looks like this: 
  //    urn:dslforum-org:device:InternetGatewayDevice:1 
  std::string r = full_type_name;

  auto pos_end = full_type_name.rfind(':');
  if ( pos_end != std::string::npos )
  {
    auto pos_begin = full_type_name.rfind(':', pos_end-1);
    r = full_type_name.substr(pos_begin+1, pos_end-1 - pos_begin);
  }

  return r;
}
