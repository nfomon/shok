// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

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
