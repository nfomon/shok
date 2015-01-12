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

  Log(LEVEL level = INFO);
  Log(const std::string& logfile, LEVEL level = INFO);
  ~Log();

  void Init(const std::string& logfile);

  void setLevel(LEVEL level);
  void setLevel(const std::string& level);

  std::ofstream& error();
  std::ofstream& warning();
  std::ofstream& info();
  std::ofstream& debug();

private:
  LEVEL m_level;
  std::ofstream m_log;
  std::ofstream m_null;
};

#endif // _Log_h_
