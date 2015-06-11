#ifndef TR064EXCEPTION_HH
#define TR064EXCEPTION_HH

#include <stdexcept>
#include <string>

namespace tr064
{
class Tr064Exception : 
  public std::runtime_error
{
public:
  Tr064Exception(const std::string& info) : 
    std::runtime_error(info) 
  { }

  //~Tr064Exception() throw () {}
};

}


#endif /* TR064EXCEPTION_HH */
