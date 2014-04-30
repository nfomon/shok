// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Object_h_
#define _Object_h_

/* Object */

#include "SymbolTable.h"

#include <memory>
#include <string>

namespace vm {

class Object {
public:
  Object();

  bool operator< (const Object& rhs) const {
    return this < &rhs;
  }

private:
  // parents
  // members
  symbol_map m_members;
};

}

#endif // _Object_h_
