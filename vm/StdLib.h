// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _StdLib_h_
#define _StdLib_h_

/* A runtime symbol table */

#include "SymbolTable.h"

#include <map>
#include <string>

namespace vm {

class StdLib {
public:
  static void Initialize(symbol_map& symbols);
};

}

#endif // _StdLib_h_
