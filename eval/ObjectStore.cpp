// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "ObjectStore.h"

#include "EvalError.h"

#include <boost/lexical_cast.hpp>

#include <string>
using std::string;

using namespace eval;

ObjectStore::~ObjectStore() {
  reset();
}

// Clears all objects from the store
void ObjectStore::reset() {
  for (object_iter i = m_objects.begin(); i != m_objects.end(); ++i) {
    delete i->second;
  }
  m_objects.clear();
  m_pending.clear();
}

// Commit (confirm) a pending object into the store
void ObjectStore::commit(const string& varname) {
  if (m_pending.end() == m_pending.find(varname)) {
    throw EvalError("Cannot commit " + varname + "; object missing");
  }
  m_log.debug("Committing " + varname);
  m_pending.erase(varname);
}

// Commit all pending-commit objects
void ObjectStore::commitAll() {
  m_log.debug("Committing all variables");
  for (object_iter i = m_pending.begin(); i != m_pending.end(); ++i) {
    commit(i->first);
  }
}

// Revert a pending-commit object
void ObjectStore::revert(const string& varname) {
  object_iter o = m_objects.find(varname);
  object_iter p = m_pending.find(varname);
  if (m_objects.end() == o || m_pending.end() == p) {
    throw EvalError("Cannot revert " + varname + "; object missing");
  }
  m_log.info("Reverting " + varname);
  delete o->second;
  m_objects.erase(varname);
  m_pending.erase(varname);
}

// Revert all pending-commit objects
void ObjectStore::revertAll() {
  m_log.info("Reverting all variables");
  for (object_iter i = m_pending.begin(); i != m_pending.end(); ++i) {
    revert(i->first);
  }
}

Object* ObjectStore::getObject(const string& varname) const {
  object_iter o = m_objects.find(varname);
  if (o != m_objects.end()) return o->second;
  return NULL;
}

void ObjectStore::newObject(const string& varname, Object* object) {
  // An object name collision should have already been detected, but repeat
  // this now until we're confident about that
  if (getObject(varname)) {
    throw EvalError("Cannot create variable " + varname + "; already exists, and should never have been created");
  }
  m_log.info("Adding (pending) object " + varname + " to an object store");
  object_pair op(varname, object);
  m_objects.insert(op);
  m_pending.insert(op);
}

void ObjectStore::delObject(const string& varname) {
  m_log.info("Deleting object " + varname);
  object_iter o = m_objects.find(varname);
  if (m_objects.end() == o) {
    throw EvalError("Cannot delete variable " + varname + "; does not exist in this store");
  }
  delete o->second;
  m_objects.erase(varname);
  m_pending.erase(varname);   // if it's there
}
