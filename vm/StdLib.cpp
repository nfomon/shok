// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "StdLib.h"

#include "Object.h"
#include "VMError.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace vm;

void StdLib::Initialize(symbol_map& symbols) {
  auto_ptr<Object> object(new Object());
  symbols.insert("object", object);

  //auto_ptr<Object> function(new Object());
  //symbols.insert("@", function);
}
