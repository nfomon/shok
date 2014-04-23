// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _SymbolTable_h_
#define _SymbolTable_h_

/* SymbolTable */

#include "CompileError.h"
#include "Type.h"

#include "util/Log.h"

#include <map>
#include <string>

namespace compiler {

class SymbolTable {
public:
  SymbolTable(Log& log)
    : m_log(log) {}

  void insert(const std::string& name, const Type& type);
  const Type* find(const std::string& name) const;

private:
  typedef std::map<std::string,Type> symbol_map;
  typedef symbol_map::const_iterator symbol_iter;

  Log& m_log;
  symbol_map m_symbols;
};

}

#endif // _SymbolTable_h_
