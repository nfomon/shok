// Copyright (C) 2013 Michael Biggs.  See the COPYRIGHT file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "Variable.h"

#include "EvalError.h"

#include <string>
using std::string;

using namespace eval;

void Variable::setup() {
  if (children.size() != 0) {
    throw EvalError("Variable node cannot have children");
  }
}

void Variable::analyzeUp() {
}

void Variable::evaluate() {
}
