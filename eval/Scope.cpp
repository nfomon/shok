// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "Scope.h"

#include "EvalError.h"

#include <boost/lexical_cast.hpp>

#include <string>
using std::string;

using namespace eval;

Scope::~Scope() {
  m_log.info("Destroying scope at depth " +
             boost::lexical_cast<string>(m_depth));
  reset();
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
  for (object_iter i = m_objects.begin(); i != m_objects.end(); ++i) {
    delete i->second;
  }
  m_objects.clear();
  m_pending.clear();
}

// Commit (confirm) a pending object into the scope
void Scope::commit(const string& varname) {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->commit(varname);
  }
  if (m_pending.end() == m_pending.find(varname)) {
    throw EvalError("Cannot commit " + varname + " to scope depth " +
                    boost::lexical_cast<string>(m_depth) + "; object missing");
  }
  m_log.debug("Committing " + varname + " to scope depth " +
             boost::lexical_cast<string>(m_depth));
  m_pending.erase(varname);
}

// Commit all pending-commit objects
void Scope::commitAll() {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->commitAll();
  }
  m_log.info("Committing all variables from scope depth " +
             boost::lexical_cast<string>(m_depth));
  for (object_iter i = m_pending.begin(); i != m_pending.end(); ++i) {
    commit(i->first);
  }
}

// Revert a pending-commit object
void Scope::revert(const string& varname) {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->revert(varname);
  }
  object_iter o = m_objects.find(varname);
  object_iter p = m_pending.find(varname);
  if (m_objects.end() == o || m_pending.end() == p) {
    throw EvalError("Cannot revert " + varname + " from scope depth " +
                    boost::lexical_cast<string>(m_depth) + "; object missing");
  }
  m_log.info("Reverting " + varname + " from scope depth " +
             boost::lexical_cast<string>(m_depth));
  delete o->second;
  m_objects.erase(varname);
  m_pending.erase(varname);
}

// Revert all pending-commit objects
void Scope::revertAll() {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->revertAll();
  }
  m_log.info("Reverting all variables from scope depth " +
             boost::lexical_cast<string>(m_depth));
  for (object_iter i = m_pending.begin(); i != m_pending.end(); ++i) {
    revert(i->first);
  }
}

Object* Scope::getObject(const string& varname) const {
  object_iter o = m_objects.find(varname);
  if (o != m_objects.end()) return o->second;
  if (!m_parentScope) {
    return NULL;
  }
  return m_parentScope->getObject(varname);
}

void Scope::newObject(const string& varname, Object* object) {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->newObject(varname, object);
  }
  // An object name collision should have already been detected, but repeat
  // this now until we're confident about that
  if (getObject(varname)) {
    throw EvalError("Cannot create variable " + varname + "; already exists in this scope, and should never have been created");
  }
  m_log.info("Adding (pending) object " + varname +
             " at scope depth " + boost::lexical_cast<string>(m_depth));
  object_pair op(varname, object);
  m_objects.insert(op);
  m_pending.insert(op);
  m_log.info("Scope depth " + boost::lexical_cast<string>(m_depth) +
             " now has " + boost::lexical_cast<string>(m_objects.size()) +
             " objects and " + boost::lexical_cast<string>(m_pending.size()) +
             " pending");
}

void Scope::delObject(const string& varname) {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->delObject(varname);
  }
  m_log.info("Deleting object " + varname +
             " from scope depth " + boost::lexical_cast<string>(m_depth));
  object_iter o = m_objects.find(varname);
  if (m_objects.end() == o) {
    throw EvalError("Cannot delete variable " + varname +
                    "; does not exist in this scope");
  }
  delete o->second;
  m_objects.erase(varname);
  m_pending.erase(varname);   // if it's there
}
