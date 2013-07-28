// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "Expression.h"

#include "EvalError.h"
#include "Operator.h"

#include <string>
using std::string;

using namespace eval;

void Expression::setup() {
  if (children.size() != 1) {
    throw EvalError("Expression must wrap a single expression fragment");
  }

  Operator* op = dynamic_cast<Operator*>(children.at(0));
  if (op) {
    op->analyzeTree();
  }
}

void Expression::evaluate() {
  // TODO: call the resulting object's ->str()->escape() (*UNIMPL*)
  throw EvalError("Cannot evaluate expression: unimplemented");
  //m_str = children.front()->cmdText();
}

string Expression::cmdText() const {
  if (!isEvaluated) {
    throw EvalError("Cannot get cmdText of unevaluated Expression");
  }
  return m_str;
}
