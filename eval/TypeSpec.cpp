// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "TypeSpec.h"

#include "EvalError.h"
#include "Operator.h"

#include <string>
using std::string;

using namespace eval;

void TypeSpec::setup() {
  if (children.size() != 1) {
    throw EvalError("TypeSpec must wrap a single expression fragment");
  }

  Operator* op = dynamic_cast<Operator*>(children.at(0));
  if (op) {
    op->analyzeTree();
  }
  computeType();
}

// Nothing to do here
void TypeSpec::evaluate() {
}

void TypeSpec::computeType() {
  // Extract the type of our expression tree, then delete all its nodes; they
  // are irrelevant, we don't want to actually evaluate them as if they were
  // code.
  TypedNode* child = dynamic_cast<TypedNode*>(children.at(0));
  if (!child) {
    throw EvalError("Child of TypeSpec must be a TypedNode");
  }
  m_type = child->getType();
  if (!m_type.get()) {
    throw EvalError("Child of TypeSpec must have a Type");
  }
  delete children.at(0);
  children.clear();
}
