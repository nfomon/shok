// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Log.h"

#include <iostream>
#include <stdexcept>
#include <string>
using std::string;

Log::Log(const string& logfile, const LEVEL level)
  : m_level(level) {
  m_log.open(logfile.c_str());
  if (!m_log) {
    throw std::runtime_error("Failed to open logfile " + logfile);
  }
  info("Initialized log at level " + UnmapLevel(level));
}

Log::~Log() {
  debug("Destroying log");
  m_log.close();
}

void Log::setLevel(LEVEL level) {
  m_level = level;
}

void Log::setLevel(const string& level) {
  if ("ERROR" == level) {
    setLevel(ERROR);
  } else if ("WARNING" == level) {
    setLevel(WARNING);
  } else if ("INFO" == level) {
    setLevel(INFO);
  } else if ("DEBUG" == level) {
    setLevel(DEBUG);
  } else {
    throw std::runtime_error("Cannot set log to unknown level '" + level + "'");
  }
}

void Log::error(const string& msg) {
  if (m_level > ERROR) return;
  m_log << "ERROR:   " << msg << std::endl;
  std::cerr << "ERROR:   " << msg << std::endl;
}

void Log::warning(const string& msg) {
  if (m_level > WARNING) return;
  m_log << "WARNING: " << msg << std::endl;
  std::cerr << "WARNING: " << msg << std::endl;
}

void Log::info(const string& msg) {
  if (m_level > INFO) return;
  m_log << "INFO:    " << msg << std::endl;
}

void Log::debug(const string& msg) {
  if (m_level > DEBUG) return;
  m_log << "DEBUG:   " << msg << std::endl;
}
