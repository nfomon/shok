// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "SymbolTable.h"

#include "VMError.h"

#include <map>
#include <string>
using std::map;
using std::string;

using namespace vm;

SymbolTable::SymbolTable(Log& log)
  : m_log(log) {
}
