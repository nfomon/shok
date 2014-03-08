// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Log_h_
#define _Log_h_

/* Debug log */

#include <boost/lexical_cast.hpp>

#include <exception>
#include <fstream>
#include <string>

class Log {
public:
  enum LEVEL {
    DEBUG = 10,
    INFO = 20,
    WARNING = 30,
    ERROR = 40
  };
  std::string UnmapLevel(LEVEL level) const {
    switch (level) {
      case DEBUG:   return "debug";
      case INFO:    return "info";
      case WARNING: return "warning";
      case ERROR:   return "error";
      default: throw std::runtime_error("Cannot unmap level " + boost::lexical_cast<std::string>(level));
    }
  }

  Log(const std::string& logfile, const LEVEL level = DEBUG);
  ~Log();

  void setLevel(LEVEL level);
  void setLevel(const std::string& level);

  void error(const std::string& msg);
  void warning(const std::string& msg);
  void info(const std::string& msg);
  void debug(const std::string& msg);

private:
  std::ofstream m_log;
  LEVEL m_level;
};

#endif // _Log_h_
