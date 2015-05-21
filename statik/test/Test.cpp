// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Test.h"

#include "STLog.h"

#include <string>
using std::string;

using namespace statik_test;

/* public */

void Test::Init() {
  g_log.info() << "Initializing test" << (m_name.empty() ? "" : ": " + m_name);
  init();
}

void Test::Run() {
  g_log.info() << "Running test" << (m_name.empty() ? "" : ": " + m_name);
  run();
  g_log.info() << "Done test";
  g_log.info();
}

/* protected */

void Test::pass(const std::string& msg) {
  ++m_result.pass;
  g_log.info() << "pass" << (msg.empty() ? "" : ": " + msg);
}

void Test::fail(const std::string& msg) {
  ++m_result.fail;
  g_log.info() << "fail" << (msg.empty() ? "" : ": " + msg);
}

void Test::test(bool t, const std::string& msg) {
  t ? pass(msg) : fail(msg);
}
