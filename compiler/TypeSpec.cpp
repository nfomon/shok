// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "TypeSpec.h"

#include "CompileError.h"
#include "Operator.h"

#include <string>
using std::string;

using namespace compiler;

void TypeSpec::setup() {
  if (children.size() != 1) {
    throw CompileError("TypeSpec must wrap a single expression fragment");
  }
  computeType();
}

string TypeSpec::getTypeName() const {
  return m_type->getName();
}

void TypeSpec::computeType() {
  // Extract the type of our expression tree, then delete all its nodes; they
  // are irrelevant, we don't want to actually compile them as if they were
  // code.
  TypedNode* child = dynamic_cast<TypedNode*>(children.at(0));
  if (!child) {
    throw CompileError("Child of TypeSpec must be a TypedNode");
  }
  m_type = child->getType();
  if (!m_type.get()) {
    throw CompileError("Child of TypeSpec must have a Type");
  }
  delete children.at(0);
  children.clear();
}
