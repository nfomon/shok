// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "IsVar.h"

#include "EvalError.h"
#include "Variable.h"

#include <iostream>
#include <string>
using std::string;

using namespace eval;

void IsVar::setup() {
  if (children.size() != 1) {
    throw EvalError("IsVar must have a single child");
  }
  Variable* var = dynamic_cast<Variable*>(children.at(0));
  if (!var) {
    throw EvalError("IsVar must have a single Variable as child");
  }
  bool isvar = parentScope->getObject(var->getVariableName()) != NULL;
  std::cout << "PRINT:" << (isvar ? "true" : "false") << std::endl;
}

// Nothing to do here
void IsVar::evaluate() {
}
