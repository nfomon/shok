// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Symbol_h_
#define _Symbol_h_

/* Symbol
 *
 * A Symbol is a typed entry in a symbol table.  The "Type" just refers to the
 * set of parent Symbol(s).  The Symbol itself has a SymbolTable of members.
 * At compile()-time it will be given an actual Object instance.
 */

#include "Common.h"
#include "SymbolTable.h"

#include "util/Log.h"

#include <map>
#include <memory>
#include <string>

namespace compiler {

class Object;
class Type;

class Symbol {
public:
  Symbol(Log& log, std::auto_ptr<Type> type)
    : log(log), type(type), object(NULL) {}
  Log& log;
  std::auto_ptr<Type> type;
  std::auto_ptr<Object> object;
};

}

#endif // _Symbol_h_
