// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "ObjectStore.h"

#include "EvalError.h"
#include "Object.h"

#include <boost/lexical_cast.hpp>

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace eval;

ObjectStore::~ObjectStore() {
  reset();
}

// Clears all objects from the store
void ObjectStore::reset() {
  revertAll();
  for (ordering_rev_mod_iter i = m_ordering.rbegin(); i != m_ordering.rend(); ++i) {
    object_mod_iter o = m_objects.find(i->first);
    if (m_objects.end() == o) {
      throw EvalError("Resetting ObjectStore failed to find " + i->first);
    }
    o->second->destruct();
    delete o->second;
    m_objects.erase(o);
  }
  m_ordering.clear();
  if (!m_objects.empty()) {
    throw EvalError("Resetting ObjectStore failed to clear all objects");
  }
}

// Commit (confirm) the oldest pending change to the store
void ObjectStore::commitFirst() {
  if (m_changeset.empty()) {
    throw EvalError("Cannot commit first of changeset; no changes pending");
  }
  ObjectChange oc = m_changeset.front();
  object_iter o = m_objects.find(oc.varname);
  if (m_objects.end() == o) {
    throw EvalError("Cannot commit " + oc.varname + "; object missing");
  }
  switch (oc.type) {
    case OBJECT_ADD: {
      m_log.debug("Commit: addition of " + oc.varname);
      o->second->construct();
    } break;
    case OBJECT_DELETE: {
      m_log.debug("Commit: deletion of " + oc.varname);
      if (!oc.oldObject) {
        throw EvalError("Cannot commit deletion of " + oc.varname + "; old object missing from changeset");
      }
      bool found = false;
      for (ordering_mod_iter i = m_ordering.begin(); i != m_ordering.end(); ++i) {
        if ((oc.varname == i->first) && i->second) {
          m_ordering.erase(i);
          found = true;
          break;
        }
      }
      if (!found) {
        throw EvalError("Cannot commit deletion of " + oc.varname + "; object is missing from ordering");
      }
      oc.oldObject->destruct();
      delete oc.oldObject;
    } break;
    default: throw EvalError("Unsupported ObjectStore change type");
  }
  m_changeset.pop_front();
}

// Commit all pending-commit objects
void ObjectStore::commitAll() {
  m_log.debug("Committing all changes");
  for (int i=0; i < m_changeset.size(); ++i) {
    commitFirst();
  }
}

// Revert the newest pending-commit change from the store
void ObjectStore::revertLast() {
  if (m_changeset.empty()) {
    throw EvalError("Cannot revert last of changeset; no changes pending");
  }
  ObjectChange oc = m_changeset.back();
  object_mod_iter o = m_objects.find(oc.varname);
  if (m_objects.end() == o) {
    throw EvalError("Cannot revert " + oc.varname + "; object missing");
  }
  switch (oc.type) {
    case OBJECT_ADD: {
      m_log.debug("Revert: addition of " + oc.varname);
      // The add to revert should be the last element of m_ordering
      if (m_ordering.empty()) {
        throw EvalError("Cannot revert addition of " + oc.varname + "; object is missing from ordering");
      }
      ordering_pair order = m_ordering.back();
      if (order.first != oc.varname) {
        throw EvalError("Cannot revert addition of " + oc.varname + "; object missing or misplaced in ordering");
      } else if (order.second) {
        throw EvalError("Cannot revert addition of " + oc.varname + "; ordering suggests object is pending deletion");
      }
      m_ordering.pop_back();
      m_objects.erase(o);
    } break;
    case OBJECT_DELETE: {
      m_log.debug("Revert: deletion of " + oc.varname);
      if (!oc.oldObject) {
        throw EvalError("Cannot revert deletion of " + oc.varname + "; old object missing from changeset");
      }
      bool found = false;
      for (ordering_rev_mod_iter i = m_ordering.rbegin(); i != m_ordering.rend(); ++i) {
        if (oc.varname == i->first) {
          if (!i->second) {
            throw EvalError("Cannot revert deletion of " + oc.varname + "; ordering suggests object is not pending deletion");
          }
          i->second = false;
          found = true;
          break;
        }
      }
      if (!found) {
        throw EvalError("Cannot revert deletion of " + oc.varname + "; object is missing from ordering");
      }
      object_pair op(oc.varname, oc.oldObject);
      m_objects.insert(op);
    } break;
    default: throw EvalError("Unsupported ObjectStore change type");
  }
  m_changeset.pop_back();
}

// Revert all pending-commit objects
void ObjectStore::revertAll() {
  for (int i=0; i < m_changeset.size(); ++i) {
    revertLast();
  }
}

Object* ObjectStore::getObject(const string& varname) const {
  object_iter o = m_objects.find(varname);
  if (o != m_objects.end()) return o->second;
  return NULL;
}

Object& ObjectStore::newObject(const string& varname, auto_ptr<Type> type) {
  // An object name collision should have already been detected, but repeat
  // this now until we're confident about that
  if (getObject(varname)) {
    throw EvalError("Cannot create variable " + varname + "; already exists, and should never have been created");
  }
  m_log.info("Adding (pending) object " + varname + " to an object store");
  object_pair op(varname, new Object(m_log, varname, type));
  m_objects.insert(op);
  // slow paranoid safety check
  for (ordering_iter i = m_ordering.begin(); i != m_ordering.end(); ++i) {
    if (varname == i->first && !i->second) {
      throw EvalError("Cannot create variable " + varname + "; somehow already exists in ordering");
    }
  }
  ordering_pair orp(varname, false);
  m_ordering.push_back(orp);
  ObjectChange oc(varname, OBJECT_ADD);
  m_changeset.push_back(oc);
  return *op.second;
}

void ObjectStore::delObject(const string& varname) {
  m_log.info("Deleting object " + varname);
  object_mod_iter o = m_objects.find(varname);
  if (m_objects.end() == o) {
    throw EvalError("Cannot delete variable " + varname + "; does not exist in this store");
  }
  ObjectChange oc(varname, OBJECT_DELETE, o->second);
  bool found = false;
  for (ordering_mod_iter i = m_ordering.begin(); i != m_ordering.end(); ++i) {
    if ((varname == i->first) && !i->second) {
      i->second = true;
      found = true;
      break;
    }
  }
  if (!found) {
    throw EvalError("Cannot delete variable " + varname + "; somehow does not exist in ordering");
  }
  m_objects.erase(o);
  m_changeset.push_back(oc);
}

void ObjectStore::replaceObject(const string& varname, auto_ptr<Object> newObject) {
  m_log.info("Replacing object " + varname);
  object_mod_iter o = m_objects.find(varname);
  if (m_objects.end() == o) {
    throw EvalError("Cannot replace variable " + varname + "; does not exist in this store");
  } else if (!o->second) {
    throw EvalError("Cannot replace variable " + varname + "; found deficient object in the store");
  } else if (!newObject.get()) {
    throw EvalError("Cannot replace variable " + varname + " with no newObject");
  }
  if (varname != newObject->getName()) {
    m_log.warning("Replacing ObjectStore name " + varname + " with object that thinks its name is " + newObject->getName());
  }
  o->second->destruct();
  delete o->second;
  o->second = newObject.release();
}

auto_ptr<ObjectStore> ObjectStore::duplicate() const {
  m_log.debug("Duplicating object store");
  auto_ptr<ObjectStore> os(new ObjectStore(m_log));
  for (object_iter i = m_objects.begin(); i != m_objects.end(); ++i) {
    os->newObject(i->first, i->second->getType().duplicate());
  }
  os->commitAll();
  for (object_iter i = m_objects.begin(); i != m_objects.end(); ++i) {
    os->replaceObject(i->first, i->second->clone(i->first));
  }
  return os;
}
