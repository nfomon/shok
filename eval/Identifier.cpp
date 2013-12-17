// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Identifier.h"

#include "EvalError.h"

#include <string>
using std::string;

using namespace eval;

void Identifier::setup() {
  if (children.size() != 0) {
    throw EvalError("Identifier node cannot have children");
  }
  if ("" == value) {
    throw EvalError("Identifier cannot have blank value");
  }
}

// Nothing to do
void Identifier::evaluate() {
}
