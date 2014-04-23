// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "SymbolTable.h"

#include "CompileError.h"

#include "util/Util.h"

#include <map>
#include <string>
#include <utility>
using std::map;
using std::string;

using namespace compiler;

void SymbolTable::insert(const string& name, const Type& type) {
  if (m_symbols.find(name) != m_symbols.end()) {
    throw CompileError("Cannot insert symbol " + name + " into symbol table; already exists");
  }
  m_symbols.insert(std::pair<string,Type>(name, type));
}

const Type* SymbolTable::find(const string& name) const {
  symbol_iter s = m_symbols.find(name);
  if (s != m_symbols.end()) {
    return &s->second;
  }
  return NULL;
}
