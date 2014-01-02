// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Returns.h"

#include "EvalError.h"
#include "TypeSpec.h"

#include <string>
#include <vector>
using std::string;
using std::vector;

using namespace eval;

void Returns::setup() {
  if (children.size() != 1) {
    throw EvalError("TypeSpec" + print() + " must have 1 child");
  }
  m_typeSpec = dynamic_cast<TypeSpec*>(children.at(0));
  if (!m_typeSpec) {
    throw EvalError("Arg " + print() + " child must be a TypeSpec");
  }
}

// Nothing to do
void Returns::evaluate() {
}

string Returns::getName() const {
  if (!isSetup || !m_typeSpec) {
    throw EvalError("Cannot get name of Function-Returns "  + print() + " that has not been setup");
  }
  return m_typeSpec->getTypeName();
}
