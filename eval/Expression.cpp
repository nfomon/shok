// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Expression.h"

#include "EvalError.h"
#include "Function.h"
#include "Operator.h"
#include "Variable.h"

#include <boost/lexical_cast.hpp>

#include <memory>
#include <string>
#include <vector>
using std::auto_ptr;
using std::string;
using std::vector;

using namespace eval;

void Expression::setup() {
  if (children.size() != 1) {
    throw EvalError("Expression " + print() + " must have exactly one child");
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
  return "Expression__cmdText unimplemented";
}

auto_ptr<Object> Expression::getObject(const string& newName) const {
  if (!isEvaluated) {
    throw EvalError("Cannot get object from Expression " + print() + " before its evaluation");
  } else if (0 == children.size()) {
    throw EvalError("Cannot get object from defective Expression " + print() + " with no children");
  }
  Variable* var = dynamic_cast<Variable*>(children.at(0));
  Operator* op = dynamic_cast<Operator*>(children.at(0));
  Function* function = dynamic_cast<Function*>(children.at(0));
  //ObjectLiteral* function = dynamic_cast<ObjectLiteral*>(children.at(0)); // TODO
  if (var) {
    auto_ptr<Object> tmp = var->getObject().clone(newName);
    return tmp;
  // } else if (op) {   // TODO
  } else if (function) {
    return function->makeObject(newName);
  }
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
