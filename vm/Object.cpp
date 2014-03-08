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

Object::Object(const Object& source) {
}
