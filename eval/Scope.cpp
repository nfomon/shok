// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "Scope.h"

#include "EvalError.h"

#include <boost/lexical_cast.hpp>

#include <string>
using std::string;

using namespace eval;

Scope::~Scope() {
  m_log.info("Destroying scope at depth " + boost::lexical_cast<string>(m_depth));
  reset();
}

// Remains the root scope if this is never called
void Scope::init(Scope* parentScope) {
  if (!parentScope) {
    throw EvalError("Cannot init Scope with NULL parentScope");
  }
  m_parentScope = parentScope;
  m_depth = parentScope->m_depth + 1;
}

void Scope::reset() {
  for (object_iter i = m_objects.begin(); i != m_objects.end(); ++i) {
    delete i->second;
  }
  m_pending.clear();
}

// Currently, only the RootNode bothers to do this; other scopes are held by
// nodes that just get deleted whether they ran successfully or not.  This will
// change when we start responding to errors better, and we'll have to do a
// more intelligent changeset-revert.
void Scope::cleanup() {
  m_pending.clear();
}

bool Scope::hasObject(const string& varname) const {
  if (m_objects.find(varname) != m_objects.end()) {
    return true;
  } else if (m_parentScope) {
    return m_parentScope->hasObject(varname);
  }
  return false;
}

Object* Scope::newObject(const string& varname) {
  // Not the way I wanted to do this, but fine.
  // depth of 1 just defers up to the root scope
  if (1 == m_depth) {
    if (!m_parentScope) {
      throw EvalError("Internal Scope error: depth of 1 but no parent Scope");
    }
    return m_parentScope->newObject(varname);
  }
  if (hasObject(varname)) {
    throw EvalError("Cannot create variable " + varname + "; already exists in this scope; " + boost::lexical_cast<string>(m_objects.size()));
  }
  m_log.info("Creating (pending) object " + varname + " at scope depth " + boost::lexical_cast<string>(m_depth));
  Object* o = new Object();
  object_pair op(varname, o);
  m_objects.insert(op);
  m_pending.insert(op);
  return o;
}

void Scope::delObject(const string& varname) {
  if (m_objects.end() == m_objects.find(varname)) {
    throw EvalError("Cannot delete variable " + varname + "; does not exist in this scope");
  }
  m_objects.erase(varname);
  m_pending.erase(varname);   // if it's there
}
