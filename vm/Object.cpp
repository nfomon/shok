// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Object.h"

#include "VMError.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace vm;

void Object::assign(const string& name, auto_ptr<Object> value) {
  symbol_iter m = m_members.find(name);
  if(m_members.end() == m) {
    throw VMError("Cannot assign to member " + name + "; no such member found on object");
  }
  m_members.insert(name, value);  // TODO use iterator
}
