// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Block.h"

#include "EvalError.h"
#include "Expression.h"
#include "Scope.h"

#include <string>
using std::string;

using namespace eval;

Block::~Block() {
  log.debug("Destroying Block " + print());
  m_scope.reset();
}

void Block::initScope(Scope* scopeParent) {
  m_scope.init(scopeParent);
}

void Block::initScope(Scope* scopeParent, Function* function) {
  m_scope.init(scopeParent, function);
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

// Our children have already been evaluated.  This evaluation time is what
// occurs at the closing }.  We destroy all objects in this scope.
void Block::evaluate() {
  m_scope.reset();
}

string Block::cmdText() const {
  if (!m_exp) {
    throw EvalError("Cannot get cmdText of a code block");
  }
  return m_exp->cmdText();
}
