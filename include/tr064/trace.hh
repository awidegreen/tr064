#ifndef TRACE_HH
#define TRACE_HH

#include <iostream>
#include <string.h>

#ifndef TR064_DEBUG 
#define TR064_DEBUG 0
#endif

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : \
    strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

#define LOG_DEBUG(x) \
  if (TR064_DEBUG) \
  {\
    LOG_GENERIC(Logging::LOG_LEVEL_DEBUG, "debug", x) \
  }

#define LOG_TRACE(x) \
  LOG_GENERIC(Logging::LOG_LEVEL_TRACE, "trace", x) 

#define LOG_GENERIC(_level, level_name, x) \
  if (Logging::getInstance()->level() >= _level || _level == Logging::LOG_LEVEL_DEBUG) \
  {\
    std::cerr << "[LOG:" << level_name << "] " <<  __FILENAME__ << ":" << __LINE__ << " : " << x << std::endl; \
  }


class Logging
{
public:
  enum LOG_LEVEL { LOG_LEVEL_NONE, LOG_LEVEL_TRACE, LOG_LEVEL_DEBUG };

  Logging(LOG_LEVEL level) : _log_level(level) { }
  static Logging* getInstance() 
  { 
    if (!_instance)
      _instance = new Logging(LOG_LEVEL_NONE);
    return _instance;
  }
  inline LOG_LEVEL level() const { return _log_level; }
  void level(LOG_LEVEL level) { _log_level = level; }

private:
  LOG_LEVEL _log_level;
  static Logging* _instance;
};

 
    
#endif /* TRACE_HH */

