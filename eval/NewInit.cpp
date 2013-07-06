#include "NewInit.h"

#include "Block.h"
#include "EvalError.h"
#include "Type.h"
#include "Variable.h"

#include <string>
using std::string;

using namespace eval;

void NewInit::setup() {
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    if (!dynamic_cast<Variable*>(*i)) {
      throw EvalError("NewInit children must all be Variables");
    }
  }
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

void NewInit::analyze() {
  // Make sure there's no conflicting name in scope
  if (!block) {
    throw EvalError("Cannot analyze NewInit " + print() + " without a block");
  }
  if (block->isInScope(m_var)) {
    throw EvalError("Variable " + m_var->print() + " already exists");
  }
  block->addVariable(m_var);
}

void NewInit::evaluate() {
}
