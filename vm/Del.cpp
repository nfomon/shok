// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Del.h"

#include "VMError.h"

#include "util/Util.h"

// debug
#include <iostream>
using std::cout;
using std::endl;

using namespace vm;

void Del::exec(symbol_map& symbols) const {
  cout << "Del: name=" << name << endl;
  symbol_mod_iter s = symbols.find(name);
  if (symbols.end() == s) {
    throw VMError("No such symbol " + name + " to delete");
  }
  symbols.erase(s);
}
