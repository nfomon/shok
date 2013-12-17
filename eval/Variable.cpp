// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Variable.h"

#include "EvalError.h"
#include "Identifier.h"

#include <string>
#include <vector>
using std::string;
using std::vector;

using namespace eval;

void Variable::setup() {
  if (children.size() < 1) {
    throw EvalError("Variable node must have >= 1 children");
  }
  Object* current = NULL;
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    Identifier* ident = dynamic_cast<Identifier*>(*i);
    if (!ident) {
      throw EvalError("Variable children must all be Identifiers");
    }
    if (!current) {
      m_varname = ident->getName();
      current = parentScope->getObject(ident->getName());
    } else {
      m_varname += "." + ident->getName();
      current = current->getMember(ident->getName());
    }
  }
  m_object = current;
  if (!m_object) {
    throw EvalError("Object " + m_varname + " does not exist");
  }
  computeType();
}

// Nothing to do
void Variable::evaluate() {
}

Object& Variable::getObject() const {
  if (!m_object) {
    throw EvalError("Cannot retrieve Object of deficient Variable " + print());
  }
  return *m_object;
}

void Variable::computeType() {
  if (!m_object) {
    throw EvalError("Failed to find object behind Variable " + print());
  }
  m_type.reset(new BasicType(*m_object));
}
