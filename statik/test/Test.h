// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_test_Test_h_
#define _statik_test_Test_h_

#include "STLog.h"

#include <string>

namespace statik_test {

class Result {
public:
  Result()
    : pass(0), expect_fail(0), fail(0), error(0) {
  }
  unsigned pass;
  unsigned expect_fail;
  unsigned fail;
  unsigned error;
};

class Test {
public:
  Test(const std::string& name)
    : m_name(name) {}
  void Init();
  void Run();
  std::string Name() const { return m_name; }
  const Result& GetResult() const { return m_result; }

protected:
  virtual void init() {}
  virtual void run() = 0;

  void pass(const std::string& msg = "");
  void fail(const std::string& msg = "");
  void test(bool t, const std::string& msg = "");
  template <typename T>
  void test(T actual, T expected, const std::string& msg = "") {
    (actual == expected) ? pass(msg) : fail(msg);
  }

private:
  Result m_result;
  const std::string m_name;
};

}

#endif // _statik_test_Test_h_
