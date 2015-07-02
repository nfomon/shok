// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_test_Test_h_
#define _statik_test_Test_h_

#include "STLog.h"

#include "statik/Batch.h"

#include <boost/lexical_cast.hpp>

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
  virtual ~Test() {}
  void Init();
  void Run();
  std::string Name() const { return m_name; }
  const Result& GetResult() const { return m_result; }

protected:
  virtual void init() {}
  virtual void run() = 0;

  void pass(const std::string& msg = "");
  void fail(const std::string& msg = "");
  bool test(bool t, const std::string& msg = "");
  template <typename T>
  bool test(T actual, T expected, const std::string& msg = "") {
    return test(actual == expected, msg + " (actual: " + boost::lexical_cast<std::string>(actual) + " expected: " + boost::lexical_cast<std::string>(expected) + ")");
  }

  bool test(const statik::Batch& actual, const statik::Batch& expected, const std::string& msg = "");

private:
  Result m_result;
  const std::string m_name;
};

}

#endif // _statik_test_Test_h_
