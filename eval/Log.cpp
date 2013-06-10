#include "Log.h"

#include <fstream>
#include <string>
using std::string;

namespace eval {

Log::Log() {
  m_log.open(LOGFILE.c_str());
}

Log::~Log() {
  m_log.close();
}

void Log::error(const string& msg) {
  m_log << "ERROR:   " << msg << std::endl;
}

void Log::warning(const string& msg) {
  m_log << "WARNING: " << msg << std::endl;
}

void Log::info(const string& msg) {
  m_log << "INFO:    " << msg << std::endl;
}

void Log::debug(const string& msg) {
  m_log << "DEBUG:   " << msg << std::endl;
}

};
