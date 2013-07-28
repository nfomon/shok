// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "Block.h"

#include "EvalError.h"
#include "Expression.h"

#include <string>
using std::string;

using namespace eval;

Block::~Block() {
}

void Block::init() {
  m_scope.init(parentScope);
}

void Block::setup() {
  Brace::setup();

  if (m_exp) {
    throw EvalError("Block.m_exp should not already be set when setting up");
  }
  // Determine if we're a code block or an expression block
  if (1 == children.size()) {
    m_exp = dynamic_cast<Expression*>(children.front());
  }
}

// We don't actually have to do anything here.  Nodes are evaluated
// child-first.  Our child is either a single Expression, or a list of
// statements, which will be evaluated automatically.
void Block::evaluate() {
}

string Block::cmdText() const {
  if (!m_exp) {
    throw EvalError("Cannot get cmdText of a code block");
  }
  return m_exp->cmdText();
}
