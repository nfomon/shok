// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "Scope.h"

#include "EvalError.h"

#include <boost/lexical_cast.hpp>

#include <string>
#include <string>
using std::auto_ptr;
using std::string;

using namespace eval;

Scope::~Scope() {
  m_log.info("Destroying scope at depth " +
             boost::lexical_cast<string>(m_depth));
}

// Remains the root scope if this is never called
void Scope::init(Scope* parentScope) {
  if (!parentScope) {
    throw EvalError("Cannot init the root scope");
  }
  m_parentScope = parentScope;
  m_depth = parentScope->m_depth + 1;
  m_log.debug("Init scope at depth " + boost::lexical_cast<string>(m_depth));
}

// Clears all objects from the scope
void Scope::reset() {
  m_log.debug("Resetting scope at depth " +
              boost::lexical_cast<string>(m_depth));
  m_objectStore.reset();
}

// Commit (confirm) a pending object into the scope
void Scope::commit(const string& varname) {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->commit(varname);
  }
  m_objectStore.commit(varname);
}

// Commit all pending-commit objects
void Scope::commitAll() {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->commitAll();
  }
  m_objectStore.commitAll();
}

// Revert a pending-commit object
void Scope::revert(const string& varname) {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->revert(varname);
  }
  m_objectStore.revert(varname);
}

// Revert all pending-commit objects
void Scope::revertAll() {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->revertAll();
  }
  m_objectStore.revertAll();
}

const Object* Scope::getObject(const string& varname) const {
  const Object* o = m_objectStore.getObject(varname);
  if (o) return o;
  if (!m_parentScope) return NULL;
  return m_parentScope->getObject(varname);
}

const Object& Scope::newObject(const string& varname, auto_ptr<Type> type) {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->newObject(varname, type);
  }
  return m_objectStore.newObject(varname, type);
}

void Scope::delObject(const string& varname) {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->delObject(varname);
  }
  m_objectStore.delObject(varname);
}
