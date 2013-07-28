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

bool Scope::hasObject(const string& varname) const {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->hasObject(varname);
  }
  if (m_objects.find(varname) != m_objects.end()) {
    return true;
  } else if (!m_parentScope) {
    return false;
  }
  return m_parentScope->hasObject(varname);
}

Object* Scope::newObject(const string& varname) {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->newObject(varname);
  }
  if (hasObject(varname)) {
    throw EvalError("Cannot create variable " +
                    varname + "; already exists in this scope; " +
                    boost::lexical_cast<string>(m_objects.size()));
  }
  m_log.info("Creating (pending) object " + varname +
             " at scope depth " + boost::lexical_cast<string>(m_depth));
  Object* o = new Object();
  object_pair op(varname, o);
  m_objects.insert(op);
  m_pending.insert(op);
  m_log.info("Scope depth " + boost::lexical_cast<string>(m_depth) +
             " now has " + boost::lexical_cast<string>(m_objects.size()) +
             " objects and " + boost::lexical_cast<string>(m_pending.size()) +
             " pending");
  return o;
}

void Scope::delObject(const string& varname) {
  // depth of 1 is fake; it just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) { throw EvalError("Scope at depth 1 has no parent"); }
    return m_parentScope->delObject(varname);
  }
  m_log.info("Deleting object " + varname +
             " from scope depth " + boost::lexical_cast<string>(m_depth));
  if (m_objects.end() == m_objects.find(varname)) {
    throw EvalError("Cannot delete variable " + varname +
                    "; does not exist in this scope");
  }
  m_objects.erase(varname);
  m_pending.erase(varname);   // if it's there
}
