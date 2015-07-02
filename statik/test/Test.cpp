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

bool Test::test(bool t, const std::string& msg) {
  t ? pass(msg) : fail(msg);
  return t;
}

bool Test::test(const statik::Batch& actual, const statik::Batch& expected, const string& msg) {
  return test(actual.Size(), expected.Size(), msg + " batch size");
/*
  statik::Batch::batch_iter a, e;
  for (a = actual.begin(); a != actual.end(); ++a) {
    if (expected.end() == e) break;
    const statik::Batch::batch_item& aitem = *a;
    const statik::Batch::batch_item& eitem = *e;
    if (test(aitem.op, eitem.op, msg + " batch item op")) {
      if (test(item.node, "batch insert node")) {
        test(item.node->name, string("a"), "batch insert node name");
        test(item.node->value, string(""), "batch insert node value");
      }
      test(item.op, Batch::OP_INSERT, "batch insert op");
      test(item.pos, (const List*)NULL, "batch insert pos");

    }
    ++e;
*/
}
