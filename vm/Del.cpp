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

void Del::exec(Context& context) const {
  cout << "Del: name=" << name << endl;
  symbol_mod_iter s = context.locals().find(name);
  if (context.end() == s) {
    throw VMError("No such local symbol " + name + " to delete");
  }
  context.locals().erase(s);
}
