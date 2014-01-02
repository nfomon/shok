// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Arg.h"

#include "EvalError.h"
#include "Identifier.h"
#include "TypeSpec.h"

#include <string>
#include <vector>
using std::string;
using std::vector;

using namespace eval;

void Arg::setup() {
  // If two children, the first is the name.  Otherwise, name is "".
  if (1 == children.size()) {
    m_typeSpec = dynamic_cast<TypeSpec*>(children.at(0));
  } else if (2 == children.size()) {
    m_typeSpec = dynamic_cast<TypeSpec*>(children.at(1));
    Identifier* ident = dynamic_cast<Identifier*>(children.at(0));
    if (!ident) {
      throw EvalError("First child of 2-child Arg " + print() + " must be an Identifier");
    }
    m_argname = ident->getName();
  } else {
    throw EvalError("Arg node must have 1 or 2 children");
  }
  if (!m_typeSpec) {
    throw EvalError("Arg " + print() + " must have a TypeSpec child");
  }
}

// Nothing to do
void Arg::evaluate() {
}

string Arg::getName() const {
  if (!isSetup) {
    throw EvalError("Cannot get name of Arg " + print() + " before it is setup");
  }
  return m_argname;
}
