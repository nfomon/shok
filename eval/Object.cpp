// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Object.h"

#include "ObjectStore.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace eval;

Object::Object(Log& log, const string& name, auto_ptr<Type> type)
  : m_log(log),
    m_objectStore(new ObjectStore(log)),
    m_name(name),
    m_type(type),
    m_isAbstract(false) {
}

const Type& Object::getType() const {
  if (!m_type.get()) {
    throw EvalError("Object " + print() + " does not appear to have a Type");
  }
  return *m_type.get();
}

// Clears all members from the object
void Object::reset() {
  m_log.debug("Resetting object " + print());
  m_objectStore->reset();
}

// Commit (confirm) a pending member into the object
void Object::commit(change_id id) {
  m_objectStore->commit(id);
}

// Commit all pending-commit members
void Object::commitAll() {
  m_objectStore->commitAll();
}

// Revert a pending-commit member
void Object::revert(change_id id) {
  m_objectStore->revert(id);
}

// Revert all pending-commit members
void Object::revertAll() {
  m_objectStore->revertAll();
}

Object* Object::getMember(const string& name) const {
  if (!m_type.get()) {
    throw EvalError("Cannot get member " + name + " of Object " + print() + " that has no type");
  }
  // If we don't have it, check our parent(s).
  // Note: it's up to the caller to ensure that whatever action they take on
  // the result, it should be done in the context of the child object, and not
  // the Object* they get back on its own.
  Object* o = m_objectStore->getObject(name);
  if (o) return o;
  return m_type->getMember(name);
}

auto_ptr<Type> Object::getMemberType(const string& name) const {
  if (!m_type.get()) {
    throw EvalError("Cannot get type of member " + name + " of Object " + print() + " that has no type");
  }
  const Object* o = m_objectStore->getObject(name);
  if (o) return auto_ptr<Type>(o->getType().duplicate());
  return m_type->getMemberType(name);
}

change_id Object::newMember(const string& varname, auto_ptr<Type> type) {
  return m_objectStore->newObject(varname, type);
}

/*
void Object::assign(const Expression* value) {
  m_log.warning("Object assignment is unimplemented");
}
*/

bool Object::takesArgs(const paramtype_vec& params) const {
  // TODO
  /*
  for (signature_iter i = m_signatures.begin(); i != m_signatures.end(); ++i) {
    if (i->areArgsCompatible(params)) {
      return true;
    }
  }
  */
  return false;
}

auto_ptr<Type> Object::getPossibleReturnTypes(const paramtype_vec& params) const {
  if (!takesArgs(params)) {
    throw EvalError("Function " + print() + " does not take these parameters");
  }
  auto_ptr<Type> returnTypes(NULL);
  // TODO
  /*
  for (signature_iter i = m_signatures.begin(); i != m_signatures.end(); ++i) {
    if (i->areArgsCompatible(params)) {
      returnTypes.reset(i->getReturnType());
      return returnTypes;
      // *
      if (!returnTypes.get()) {
        returnTypes.reset(i->getReturnType()->duplicate());
      } else {
        Type* rt = i->getReturnType(); //...  void??  uhhh...
        if (!rt) {
          throw EvalError("Void-returning functions not yet supported");
        }
        returnTypes.reset(OrUnion(*returnTypes.get(), *i->getReturnType()));
      }
      // *
    }
  }
  */
  return returnTypes;
}

auto_ptr<Object> Object::call(const param_vec& params) const {
  // TODO
  return auto_ptr<Object>(NULL);
}

void Object::construct() {
  //m_log.debug("Constructing children of " + print());
  // TODO: Construct children
  m_log.info("Object " + print() + ": constructor called");
}

void Object::destruct() {
  m_log.info("Object " + print() + ": destructor called");
  //m_log.debug("Destroying children of " + print());
  // TODO: Destroy children
}
