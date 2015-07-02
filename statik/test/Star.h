// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_test_Star_h_
#define _statik_test_Star_h_

#include "Test.h"

#include <string>

namespace statik_test {

class Star : public Test {
public:
  Star()
    : Test("Star") {}
  virtual ~Star() {}

protected:
  virtual void run();
};

}

#endif // _statik_test_Star_h_
