// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Log_h_
#define _Log_h_

/* Program execution log
 *
 * A file log with multiple log levels.  Before it is initialized with an
 * output file name, warnings and errors will be directed to the err stream
 * (defaults to std::cerr).
 */

#include <boost/lexical_cast.hpp>

#include <exception>
#include <fstream>
#include <iostream>
#include <string>

class Log {
public:
  enum LEVEL {
    DEBUG = 10,
    INFO = 20,
    WARNING = 30,
    ERROR = 40
  };
  static std::string UnMapLevel(LEVEL level) {
    switch (level) {
      case DEBUG:   return "debug";
      case INFO:    return "info";
      case WARNING: return "warning";
      case ERROR:   return "error";
      default: throw std::runtime_error("Cannot unmap level " + boost::lexical_cast<std::string>(level));
    }
  }

  Log(LEVEL level = WARNING, std::ostream& err = std::cerr);
  Log(const std::string& logfile, LEVEL level = INFO, std::ostream& err = std::cerr);
  ~Log();

  void Init(const std::string& logfile);

  void setLevel(LEVEL level);
  void setLevel(const std::string& level);

  std::ostream& error();
  std::ostream& warning();
  std::ostream& info();
  std::ostream& debug();

private:
  std::ostream& stream();
  LEVEL m_level;
  std::ofstream m_log;
  std::ofstream m_null;
  std::ostream& m_err;
};

#endif // _Log_h_
