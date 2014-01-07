// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Block.h"

#include "EvalError.h"
#include "Expression.h"
#include "Scope.h"

#include <string>
#include <vector>
using std::string;
using std::vector;

using namespace eval;

Block::~Block() {
  log.debug("Destroying Block " + print());
  for (statement_iter i = m_statements.begin(); i != m_statements.end(); ++i) {
    (*i)->cancelParentScopeNode();
  }
  m_scope.reset();
}

void Block::initScope(Scope* scopeParent) {
  m_scope.init(scopeParent);
}

void Block::initScope(Scope* scopeParent, Function* function) {
  m_function = function;
  m_scope.init(scopeParent, function);
}

/*
void Block::initScope(Scope* scopeParent, ObjectLiteral* object) {
  m_object = object;
  m_scope.init(scopeParent, object);
}
*/

void Block::setup() {
  Brace::setup();

  if (m_exp) {
    throw EvalError("Block.m_exp should not already be set when setting up");
  }
  // Determine if we're a code block or an expression block
  if (1 == children.size()) {
    m_exp = dynamic_cast<Expression*>(children.front());
  }
  if (!m_exp) {
    vector<child_mod_iter> instants;
    for (child_mod_iter i = children.begin(); i != children.end(); ++i) {
      Statement* statement = dynamic_cast<Statement*>(*i);
      if (!statement) {
        throw EvalError("Code-Block " + print() + " has non-Statement child " + (*i)->print());
      }
      // "Instant" statements are builtins that have already been evaluated,
      // and can be discarded.
      if (statement->isInstant()) {
        instants.push_back(i);
      } else {
        m_statements.push_back(statement);
      }
    }
    for (vector<child_mod_iter>::const_iterator i = instants.begin();
         i != instants.end(); ++i) {
      delete **i;
      children.erase(*i);
    }
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
