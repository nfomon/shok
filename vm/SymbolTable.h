// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _SymbolTable_h_
#define _SymbolTable_h_

/* Symbol table
 *
 * Tracks all variables that are known, by name and scope depth.
 */

#include "Symbol.h"

#include "util/Log.h"

#include <map>
#include <string>

namespace vm {

class SymbolTable {
public:
  SymbolTable(Log& log);

  // Add a symbol to the table.  The provided value is an object that will be
  // cloned for the initial value, or NULL to define the root object.
  Symbol& add(const std::string& name, Object* value);
  Symbol& find(const std::string& name);

private:
  Log& m_log;
  typedef std::map<std::string,Symbol*> m_symbols;
};

}

#endif // _SymbolTable_h_
