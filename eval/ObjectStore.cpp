// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "ObjectStore.h"

#include "EvalError.h"

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
void ObjectStore::commit(change_id id) {
  if (m_changeset.empty()) {
    throw EvalError("Cannot commit changeset ID " + boost::lexical_cast<string>(id) + "; no changes pending");
  } else if (NO_CHANGE == id) {
    throw EvalError("Cannot commit the no-change changeset ID");
  }
  ObjectChange oc = m_changeset.front();
  if (oc.id != id) {
    throw EvalError("Commit changeset ID " + boost::lexical_cast<string>(id) + " does not match front of changeset " + boost::lexical_cast<string>(oc.id));
  }
  object_iter o = m_objects.find(oc.varname);
  if (m_objects.end() == o) {
    throw EvalError("Cannot commit " + oc.varname + "; object missing");
  }
  switch (oc.type) {
    case OBJECT_ADD: {
      m_log.debug("Commit " + boost::lexical_cast<string>(id) + ": addition of " + oc.varname);
      o->second->construct();
    } break;
    case OBJECT_DELETE: {
      m_log.debug("Commit " + boost::lexical_cast<string>(id) + ": deletion of " + oc.varname);
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
  for (changeset_iter i = m_changeset.begin(); i != m_changeset.end(); ++i) {
    commit(i->id);
  }
}

// Revert the newest pending-commit change from the store
void ObjectStore::revert(change_id id) {
  if (m_changeset.empty()) {
    throw EvalError("Cannot revert changeset ID " + boost::lexical_cast<string>(id) + "; no changes pending");
  } else if (NO_CHANGE == id) {
    throw EvalError("Cannot revert the no-change changeset ID");
  }
  ObjectChange oc = m_changeset.back();
  if (oc.id != id) {
    throw EvalError("Revert changeset ID " + boost::lexical_cast<string>(id) + " does not match back of changeset " + boost::lexical_cast<string>(oc.id));
  }
  object_mod_iter o = m_objects.find(oc.varname);
  if (m_objects.end() == o) {
    throw EvalError("Cannot revert " + oc.varname + "; object missing");
  }
  switch (oc.type) {
    case OBJECT_ADD: {
      m_log.debug("Revert " + boost::lexical_cast<string>(id) + ": addition of " + oc.varname);
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
      m_log.debug("Revert " + boost::lexical_cast<string>(id) + ": deletion of " + oc.varname);
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
  for (changeset_rev_iter i = m_changeset.rbegin(); i != m_changeset.rend(); ++i) {
    revert(i->id);
  }
}

Object* ObjectStore::getObject(const string& varname) const {
  object_iter o = m_objects.find(varname);
  if (o != m_objects.end()) return o->second;
  return NULL;
}

change_id ObjectStore::newObject(const string& varname, auto_ptr<Type> type) {
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
  ObjectChange oc(m_lastId, varname, OBJECT_ADD);
  m_changeset.push_back(oc);
  return oc.id;
}

/*
change_id ObjectStore::replaceObject(const string& varname,
                                     auto_ptr<Object> newObject) {
  m_log.info("Replacing object " + varname);
  object_mod_iter o = m_objects.find(varname);
  if (m_objects.end() == o) {
    throw EvalError("Cannot replace variable " + varname + "; does not exist in this store");
  }
  ObjectChange oc(m_lastId, varname, OBJECT_DELETE, o->second);
  m_objects.erase(o);
  m_changeset.push_back(oc);

  object_pair op(varname, newObject.release());
  m_objects.insert(op);
  ObjectChange oc2(m_lastId, varname, OBJECT_ADD, NULL, false);
  m_changeset.push_back(oc2);
  if (oc2.id != oc.id) {
    throw EvalError("Cannot replace object with two changes that have different IDs");
  }
  return oc.id;
}
*/

change_id ObjectStore::delObject(const string& varname) {
  m_log.info("Deleting object " + varname);
  object_mod_iter o = m_objects.find(varname);
  if (m_objects.end() == o) {
    throw EvalError("Cannot delete variable " + varname + "; does not exist in this store");
  }
  ObjectChange oc(m_lastId, varname, OBJECT_DELETE, o->second);
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
  return oc.id;
}
