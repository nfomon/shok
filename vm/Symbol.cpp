// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Symbol.h"

#include "VMError.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace vm;

Symbol::Symbol(const string& name, const Object& source)
  : m_name(name) {
  // Clone the object, assign the clone to m_value
  m_value.reset(new Object(source));
}
