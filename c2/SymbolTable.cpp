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

SymbolTable::SymbolTable(const SymbolTable& s) {
  for (symbol_iter i = s.m_symbols.begin(); i != s.m_symbols.end(); ++i){
    insert(i->first, i->second->duplicate());
  }
}

SymbolTable::~SymbolTable() {
  // Need to OWN these!  So we must disallow copy-ctor
  for (symbol_iter i = m_symbols.begin(); i != m_symbols.end(); ++i) {
    delete i->second;
  }
}

void SymbolTable::insert(const string& name, std::auto_ptr<Type> type) {
  if (m_symbols.find(name) != m_symbols.end()) {
    throw CompileError("Cannot insert symbol " + name + " into symbol table; already exists");
  }
  m_symbols.insert(std::make_pair(name, type.release()));
}

const Type* SymbolTable::find(const string& name) const {
  symbol_iter s = m_symbols.find(name);
  if (s != m_symbols.end()) {
    return s->second;
  }
  return NULL;
}
