// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
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
  // Lookup the in-scope Object that we represent, if it exists
  if (m_object) {
    throw EvalError("Variable cannot be setup while it already has an object");
  }
  m_object = parentScope->getObject(getVariableName());
  log.info("Seeking Object for Variable " + print() + " -- " + (m_object ? "found" : "not found"));
  if (m_object) {
    computeType();
  }
}

// Nothing to do
void Variable::evaluate() {
}

void Variable::computeType() {
  if (!m_object) {
    throw EvalError("Cannot compute type of Variable " + print() + " for non-existant object");
  }
  m_type.reset(new BasicType(*m_object));
}
