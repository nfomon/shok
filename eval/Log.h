#ifndef _Log_h_
#define _Log_h_

/* Debug log */

#include <fstream>
#include <string>

namespace eval {

const std::string LOGFILE = "eval.log";

class Log {
public:
  Log();
  ~Log();

  void error(const std::string& msg);
  void warning(const std::string& msg);
  void info(const std::string& msg);
  void debug(const std::string& msg);

private:
  std::ofstream m_log;
};

};

#endif // _Log_h_
