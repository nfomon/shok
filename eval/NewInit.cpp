// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "NewInit.h"

#include "Block.h"
#include "EvalError.h"
#include "Type.h"
#include "Variable.h"

#include <string>
using std::string;

using namespace eval;

void NewInit::setup() {
  if (children.size() < 1 || children.size() > 3) {
    throw EvalError("NewInit node must have 1, 2, or 3 children");
  }
  Variable* var = dynamic_cast<Variable*>(children.at(0));
  if (!var) throw EvalError("NewInit's first child must be a variable");
  m_varname = var->getValue();
  switch (children.size()) {
    // new x -- type and value are both 'object'
    case 1:
      // leave m_type NULL
      // leave m_value NULL
      break;
    // new x = y -- type and initial value of x are both 'y'
    case 2: {
/*
      Variable* v = dynamic_cast<Variable*>(children.at(1));
      if (!v) {
        throw EvalError("Child " + children.at(1)->print() + " should have been a Variable");
      }
      //m_type = Type(v);
      m_value = v;
*/
      break;
    }
    // new x = y = z -- type is 'y', initial value is 'z'
    case 3: {
/*
      Variable* t = dynamic_cast<Variable*>(children.at(1));
      if (!t) {
        throw EvalError("Child " + children.at(1)->print() + " should have been a Variable");
      }
      Variable* v = dynamic_cast<Variable*>(children.at(2));
      if (!v) {
        throw EvalError("Child " + children.at(2)->print() + " should have been a Variable");
      }
      //m_type = Type(t);
      m_value = v;
*/
      break;
    }
    default:
      throw EvalError("NewInit node must have 1, 2, or 3 children");
  }
}

void NewInit::analyzeUp() {
  // Make sure there's no conflicting name in scope
  if (!parentScope) {
    throw EvalError("Cannot analyze NewInit " + print() + " without a parent scope");
  }
  if (parentScope->hasObject(m_varname)) {
    throw EvalError("Variable " + m_varname + " already exists");
  }
  parentScope->newObject(m_varname);
}

void NewInit::evaluate() {
}
