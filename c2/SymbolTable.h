// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _SymbolTable_h_
#define _SymbolTable_h_

/* SymbolTable */

#include "CompileError.h"
#include "Type.h"

#include <boost/utility.hpp>

#include <map>
#include <memory>
#include <string>

namespace compiler {

class SymbolTable {
public:
  SymbolTable() {}
  SymbolTable(const SymbolTable&);
  ~SymbolTable();
  void insert(const std::string& name, std::auto_ptr<Type> type);
  const Type* find(const std::string& name) const;
  size_t size() const { return m_symbols.size(); }

private:
  typedef std::map<std::string,Type*> symbol_map;
  typedef symbol_map::const_iterator symbol_iter;
  SymbolTable& operator=(const SymbolTable&);

  symbol_map m_symbols;
};

}

#endif // _SymbolTable_h_
