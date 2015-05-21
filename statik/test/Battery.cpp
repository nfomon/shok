// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Battery.h"

#include "Test.h"

#include <ostream>
#include <string>
using std::endl;
using std::string;

using namespace statik_test;

Battery::Battery(const std::string& name, std::istream& input, std::ostream& output)
  : m_name(name),
    m_input(input),
    m_output(output) {
}

void Battery::Run() {
  m_output << "Running battery:";
  if (!m_name.empty()) {
    m_output << " " << m_name;
  }
  m_output << endl;

  if (m_tests.empty()) {
    m_output << " - No tests." << endl;
    return;
  }

  Result r;
  bool silent = false;
  for (test_mod_iter i = m_tests.begin(); i != m_tests.end(); ++i) {
    m_output << " - " << i->Name() << " ... ";
    i->Run();
    const Result& t = i->GetResult();
    if (t.error > 0) {
      m_output << "error ";
      ++r.error;
    } else if (t.fail > 0) {
      m_output << "fail ";
      ++r.fail;
    } else if (t.expect_fail > 0) {
      m_output << "expected fail ";
      ++r.expect_fail;
    } else if (!t.error && !t.fail && !t.expect_fail && t.pass) {
      m_output << "ok ";
      ++r.pass;
    } else {
      m_output << "silent";
      silent = true;
    }

    if (!silent) {
      bool comma = false;
      m_output << "(";
      if (t.pass > 0) {
        comma = true;
        m_output << t.pass << " pass";
      }
      if (t.expect_fail > 0) {
        if (comma) { m_output << ", "; } else { comma = true; }
        m_output << t.expect_fail << " expect-fail";
      }
      if (t.fail > 0) {
        if (comma) { m_output << ", "; } else { comma = true; }
        m_output << t.fail << " fail";
      }
      if (t.error > 0) {
        if (comma) { m_output << ", "; }
        m_output << t.error << " error";
      }
      m_output << ")" << endl;
    }
  }
  m_output << "Battery done:  ";
  bool comma = false;
  if (r.pass > 0) {
    comma = true;
    m_output << r.pass << " pass";
  }
  if (r.expect_fail > 0) {
    if (comma) { m_output << ", "; } else { comma = true; }
    m_output << r.expect_fail << " expected failure" << (1 == r.expect_fail ? "" : "s");
  }
  if (r.fail > 0) {
    if (comma) { m_output << ", "; } else { comma = true; }
    m_output << r.fail << " failure" << (1 == r.fail ? "" : "s");
  }
  if (r.error > 0) {
    if (comma) { m_output << ", "; } else { comma = true; }
    m_output << r.error << " error" << (1 == r.error ? "" : "s");
  }
  m_output << "." << endl;
}
