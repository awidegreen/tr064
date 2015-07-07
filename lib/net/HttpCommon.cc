#include "HttpCommon.hh"

namespace tr064
{
  void remove_tailering_cr(std::string& str)
  {
    auto pos = str.rfind('\r');
    if ( pos != std::string::npos )
      str = str.substr(0, pos);
  }
}
