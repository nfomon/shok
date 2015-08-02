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
  g_log.info() << "pass" << (msg.empty() ? "" : ":\t" + msg);
}

void Test::fail(const std::string& msg) {
  ++m_result.fail;
  g_log.info() << "FAIL" << (msg.empty() ? "" : ":\t" + msg);
}

bool Test::test(bool t, const std::string& msg) {
  t ? pass(msg) : fail(msg);
  return t;
}

bool Test::test(const statik::Batch& actual, const statik::Batch& expected, const string& msg) {
  g_log.warning() << "one";
  bool anyfail = false;
  statik::Batch::batch_iter a, e;
  a = actual.begin();
  e = expected.begin();
  for (; a != actual.end(); ++a) {
    g_log.warning() << "two";
    if (!test(e != expected.end(), msg + " enough expected items")) {
      g_log.warning() << "three";
      anyfail = true;
      break;
    }
    const statik::Batch::BatchItem& aitem = *a;
    const statik::Batch::BatchItem& eitem = *e;
    g_log.warning() << "four";
    if (test(aitem.op, eitem.op, msg + " batch item op")) {
      if (!eitem.node) {
        g_log.warning() << "Expected batch item node is missing";
      } else if (test(aitem.node, " - batch node")) {
        anyfail |= test(aitem.node->name, eitem.node->name, " - - batch node name");
        anyfail |= test(aitem.node->value, eitem.node->value, " - - batch node value");
      } else {
        anyfail = true;
      }
      anyfail |= test(aitem.pos, eitem.pos, " - batch item pos");
    } else {
      anyfail = true;
    }
    ++e;
  }
  for (; a != actual.end(); ++a) {
    fail(msg + " extra item in batch: " + a->Print());
    anyfail = true;
  }
  for (; e != expected.end(); ++e) {
    fail(msg + " item not found in batch: " + e->Print());
    anyfail = true;
  }
  if (!anyfail) {
    pass(msg + ": batches match with no failures");
  }
  return anyfail;
}
