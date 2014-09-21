// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Char_h_
#define _Char_h_

#include "State.h"

#include <string>

namespace fw {

class CharState : public State {
public:
  CharState(const char c)
    : c(c) {}
  virtual ~CharState() {}
  const char c;

  virtual operator std::string() const { return std::string("[CharState '") + c + "':" + StateFlags() + "]"; }
};

}

#endif // _Char_h_
