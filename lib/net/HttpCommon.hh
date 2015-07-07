#ifndef HTTPCOMMON_HH
#define HTTPCOMMON_HH

#include <vector>
#include <string>

namespace tr064
{
  typedef std::string HeaderEntry;
  typedef std::vector<HeaderEntry> HeaderList;

  void remove_tailering_cr(std::string& str);
}
                                   
#endif /* HTTPCOMMON_HH */
