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
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    (*i)->cancelParentScopeNode();
  }
  m_scope.reset();
  log.debug(" - done destroying Block " + print());
}

void Block::initScope(Scope* scopeParent) {
  m_scope.init(scopeParent);
}

void Block::initScope(Scope* scopeParent, Function* function) {
  m_function = function;
  m_scope.init(scopeParent, function);
}

void Block::initScope(Scope* scopeParent, ObjectLiteral* object) {
  m_object = object;
  m_scope.init(scopeParent, object);
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
  if (!m_exp) {
    vector<child_mod_iter> instants;
    for (child_mod_iter i = children.begin(); i != children.end(); ++i) {
      Statement* statement = dynamic_cast<Statement*>(*i);
      if (!statement) continue; // Note: not all Block's children are Statements
      // "Instant" statements are builtins that have already been evaluated,
      // and can be discarded.
      if (statement->isInstant()) {
        instants.push_back(i);
      } else {
        m_statements.push_back(statement);
      }
    }
    for (vector<child_mod_iter>::const_reverse_iterator i = instants.rbegin();
         i != instants.rend(); ++i) {
      delete **i;
      children.erase(*i);
    }
  }
}

// Our children have already been evaluated.  This evaluation time is what
// occurs at the closing }.  We destroy all objects in this scope.  Unless
// we're an ObjectLiteral's block, in which case, we keep them around.
void Block::evaluate() {
  if (!m_object) {
    m_scope.reset();
  }
}

string Block::cmdText() const {
  if (!m_exp) {
    throw EvalError("Cannot get cmdText of a code block");
  }
  return m_exp->cmdText();
}

vector<NewInit*> Block::getInits() const {
  if (!m_object) {
    throw EvalError("Cannot get NewInits out of non-ObjectLiteral Block " + print());
  }
  std::vector<NewInit*> iv;
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    NewInit* init = dynamic_cast<NewInit*>(*i);
    if (!init) {
      throw EvalError("ObjectLiteral Block " + print() + " must only contain NewInits");
    }
    iv.push_back(init);
  }
  return iv;
}
