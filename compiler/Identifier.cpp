// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Identifier.h"

#include "CompileError.h"

#include <string>
using std::string;

using namespace compiler;

void Identifier::setup() {
  if (children.size() != 0) {
    throw CompileError("Identifier node cannot have children");
  }
  if ("" == value) {
    throw CompileError("Identifier cannot have blank value");
  }
}

void Identifier::compile() {
}
