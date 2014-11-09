#ifndef UTILS_HH
#define UTILS_HH

// stl
#include <string>

namespace tr064
{

  /**
   * Extracts the name from the type full specified type.
   *  urn:dslforum-org:device:InternetGatewayDevice:1  
   *   should return. 
   *  InternetGatewayDevice
   * If not found original input string will be returned.
   */
  std::string extract_type_name(const std::string& full_type_name);

};

#endif /* UTILS_HH */
