// Copyright (C) 2013 Michael Biggs.  See the COPYRIGHT file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "ExpressionBlock.h"

#include "EvalError.h"

#include <string>
using std::string;

using namespace eval;

void ExpressionBlock::setup() {
  if (children.size() != 1) {
    throw EvalError("Only a single expression is allowed in an ExpressionBlock");
  }
}

void ExpressionBlock::evaluate() {
  // TODO: call the resulting object's ->str()->escape() (*UNIMPL*)
  throw EvalError("Cannot evaluate expression-block: unimplemented");
  //m_str = children.front()->cmdText();
}

string ExpressionBlock::cmdText() const {
  if (!isEvaluated) {
    throw EvalError("Cannot get cmdText of unevaluated ExpressionBlock");
  }
  return m_str;
}
