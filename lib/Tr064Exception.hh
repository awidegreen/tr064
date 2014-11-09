#ifndef TR064EXCEPTION_HH
#define TR064EXCEPTION_HH

#include <exception>
#include <string>

namespace tr064
{
class Tr064Exception : 
  public std::exception
{
public:
  Tr064Exception(const std::string& info) : 
    _info(info) 
  { }

  ~Tr064Exception() throw () {}

  const char* what() const throw() { return _info.c_str(); }

private:
  std::string _info;
};

}


#endif /* TR064EXCEPTION_HH */
