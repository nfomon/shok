// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Test.h"

#include "Batch.h"
#include "STLog.h"
using statik::Batch;

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
  qpass();
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

void Test::qpass() {
  ++m_result.pass;
}

bool Test::qtest(bool t, const std::string& msg) {
  t ? qpass() : fail(msg);
  return t;
}

bool Test::test(const Batch& actual, const Batch& expected, const string& msg) {
  bool anyfail = false;
  Batch::batch_iter a, e;
  a = actual.begin();
  e = expected.begin();
  test(actual.Size(), expected.Size(), msg + " batch sizes");
  for (; a != actual.end(); ++a) {
    if (!qtest(e != expected.end(), msg + " too many output items")) {
      anyfail = true;
      break;
    }
    const Batch::BatchItem& aitem = *a;
    const Batch::BatchItem& eitem = *e;
    if (qtest(aitem.op, eitem.op, msg + " batch item op")) {
      if (!eitem.node) {
        g_log.warning() << "Expected batch item node is missing";
      } else if (qtest(aitem.node, " - batch node")) {
        if (Batch::OP_DELETE == aitem.op) {
          // We can't look at the deleted node's name/value, because it may
          // have been *deleted*.  It's just a handle that should not be
          // dereferenced; a label for the node that was removed.
          // We also can't directly compare the expected node with the
          // observed, because they're not the same (the Test-user is not
          // actually providing the IncParser's previously-output node, as that
          // would be inconvenient).
          // So for now, don't do a check here, we can improve the API later.
          //anyfail |= !qtest(aitem.node, eitem.node, " - - batch deleted node");
        } else {
          anyfail |= !qtest(aitem.node->name, eitem.node->name, " - - batch node name");
          anyfail |= !qtest(aitem.node->value, eitem.node->value, " - - batch node value");
        }
      } else {
        fail(msg + " emit an output item with no node");
        anyfail = true;
      }
      anyfail |= !qtest(aitem.pos, eitem.pos, " - batch item pos");
    } else {
      anyfail = true;
    }
    ++e;
  }
  for (; a != actual.end(); ++a) {
    const Batch::BatchItem& aitem = *a;
    if (Batch::OP_DELETE == aitem.op) {
      // Cannot display deleted item, as it may have been erased from memory.
      // It is only a "token".
      fail(msg + " extra deleted item in batch");
    } else {
      fail(msg + " extra item in batch: " + a->Print());
    }
    anyfail = true;
  }
  for (; e != expected.end(); ++e) {
    fail(msg + " item not found in batch: " + e->Print());
    anyfail = true;
  }
  if (anyfail) {
    fail(msg + ": failures in batch");
  }
  return !anyfail;
}
