// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_test_Or_h_
#define _statik_test_Or_h_

#include "Test.h"

#include <string>

namespace statik_test {

class Or : public Test {
public:
  Or()
    : Test("Or") {}
  virtual ~Or() {}

protected:
  virtual void run();
};

}

#endif // _statik_test_Or_h_
