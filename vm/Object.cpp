// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Object.h"

#include "VMError.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace vm;

Object::Object() {
}

Object::Object(const Object& rhs) {
  for (symbol_iter i = rhs.m_members.begin(); i != rhs.m_members.end(); ++i) {
    auto_ptr<Object> member(new Object(*i->second));
    m_members.insert(i->first, member);
  }
}
