// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "StdLib.h"

#include "Instructions.h"
#include "Object.h"
#include "VMError.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace vm;

std::string StdLib::OBJECT = "object";
std::string StdLib::FUNCTION = "@";

void StdLib::Initialize(symbol_map& symbols) {
  {
    auto_ptr<Object> object(new Object());
    symbols.insert(OBJECT, object);
  }
  symbol_iter object = symbols.find(OBJECT);

  {
    auto_ptr<Object> function(new Object(*object->second));
    symbols.insert(FUNCTION, function);
  }
  //symbol_iter function = symbols.find(FUNCTION);
}
