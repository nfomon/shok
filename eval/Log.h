// Copyright (C) 2013 Michael Biggs.  See the COPYRIGHT file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _Log_h_
#define _Log_h_

/* Debug log */

#include <fstream>
#include <string>

namespace eval {

const std::string LOGFILE = "eval.log";

class Log {
public:
  enum LEVEL {
    DEBUG = 10,
    INFO = 20,
    WARNING = 30,
    ERROR = 40
  };
  static const LEVEL DEFAULT_LEVEL = DEBUG;

  Log();
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

};

#endif // _Log_h_
