// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Char_h_
#define _Char_h_

#include "Machine.h"
#include "State.h"

namespace fw {

class CharState : public State {
public:
  CharState(const char c)
    : c(c) {}
  virtual ~CharState() {}
  const char c;
};

}

#endif // _Char_h_
