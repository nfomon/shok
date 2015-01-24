// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Log.h"

#include <iostream>
#include <stdexcept>
#include <string>
using std::cerr;
using std::endl;
using std::ostream;
using std::ofstream;
using std::string;

Log::Log(const LEVEL level, ostream& err)
  : m_level(level),
    m_err(err) {
}

Log::Log(const string& logfile, const LEVEL level, ostream& err)
  : m_level(level),
    m_err(err) {
  Init(logfile);
}

Log::~Log() {
  debug() << "Destroying log";
  m_log << endl;
  m_log.close();
}

void Log::Init(const string& logfile) {
  if (m_log) {
    debug() << "Closing former log";
    m_log.close();
  }
  m_log.open(logfile.c_str());
  if (!m_log) {
    throw std::runtime_error("Failed to open logfile " + logfile);
  }
  m_log << "Initialized log with output file " << logfile;
}

void Log::setLevel(LEVEL level) {
  m_level = level;
}

void Log::setLevel(const string& level) {
  if ("error" == level || "ERROR" == level) {
    setLevel(ERROR);
  } else if ("warning" == level || "WARNING" == level) {
    setLevel(WARNING);
  } else if ("info" == level || "INFO" == level) {
    setLevel(INFO);
  } else if ("debug" == level || "DEBUG" == level) {
    setLevel(DEBUG);
  } else {
    throw std::runtime_error("Cannot set log to unknown level '" + level + "'");
  }
}

ostream& Log::error() {
  if (m_level > ERROR) return m_null;
  ostream& log = stream();
  log << endl << "ERROR:   ";
  return log;
}

ostream& Log::warning() {
  if (m_level > WARNING) return m_null;
  ostream& log = stream();
  log << endl << "WARNING: ";
  return log;
}

ostream& Log::info() {
  if (!m_log || m_level > INFO) return m_null;
  m_log << endl << "INFO:    ";
  return m_log;
}

ostream& Log::debug() {
  if (!m_log || m_level > DEBUG) return m_null;
  m_log << endl << "DEBUG:   ";
  return m_log;
}

ostream& Log::stream() {
  if (m_log) {
    return m_log;
  }
  return m_err;
}
