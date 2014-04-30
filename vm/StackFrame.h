// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _StackFrame_h_
#define _StackFrame_h_

/* Execution stack frame */

#include "SymbolTable.h"

#include <memory>
#include <string>

namespace vm {

struct StackFrame {
  symbol_map locals;
};

}

#endif // _StackFrame_h_
