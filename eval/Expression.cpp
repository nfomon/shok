// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "Expression.h"

#include "EvalError.h"
#include "Operator.h"
#include "Variable.h"

#include <string>
using std::string;

using namespace eval;

void Expression::setup() {
  if (children.size() != 1) {
    throw EvalError("Expression must wrap a single expression fragment");
  }

  Variable* var = dynamic_cast<Variable*>(children.at(0));
  Operator* op = dynamic_cast<Operator*>(children.at(0));
  if (!var && !op) {
    throw EvalError("Expression " + print() + " does not yet support child type " + children.at(0)->print());
  }
  if (op) {
    op->analyzeTree();
  }
  computeType();
}

// Nothing to do here
void Expression::evaluate() {
}

string Expression::cmdText() const {
  if (!isEvaluated) {
    throw EvalError("Cannot get cmdText of unevaluated Expression");
  }
  // TODO: call the resulting object's ->str.escape() (*UNIMPL*)
  //return children.front()->cmdText();
  return "";
}

Object& Expression::getObject() const {
  if (!isEvaluated) {
    throw EvalError("Cannot get object from Expression " + print() + " before its evaluation");
  }
  Variable* var = dynamic_cast<Variable*>(children.at(0));
  if (var) {
    return var->getObject();
  }
  // TODO: Operator
  throw EvalError("Expression " + print() + " cannot retrieve Object from unsupported child type " + children.at(0)->print());
}

void Expression::computeType() {
  TypedNode* child = dynamic_cast<TypedNode*>(children.at(0));
  if (!child) {
    throw EvalError("Child of Expression must be a TypedNode");
  }
  m_type = child->getType();
  if (!m_type.get()) {
    throw EvalError("Child of Expression must have a Type");
  }
}
