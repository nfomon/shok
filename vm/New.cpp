// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "New.h"

#include "VMError.h"

#include "util/Util.h"

#include <memory>
using std::auto_ptr;

// debug
#include <iostream>
using std::cout;
using std::endl;

using namespace vm;

void New::exec(symbol_map& symbols) const {
  cout << "New: name=" << name << endl;
  Exec_Exp exec_Exp(symbols);
  auto_ptr<Object> value = boost::apply_visitor(exec_Exp, exp);
  if (symbols.find(name) != symbols.end()) {
    throw VMError("Cannot insert symbol " + name + "; already exists");
  }
  symbols.insert(name, value);
}
