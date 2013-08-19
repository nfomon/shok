// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "Object.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace eval;

Object::Object(Log& log, const string& name, auto_ptr<Type> type)
  : m_log(log),
    m_objectStore(new ObjectStore(log)),
    m_name(name),
    m_type(type) {
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

Object& Object::newMember(const string& varname, auto_ptr<Type> type) {
  return m_objectStore->newObject(varname, type);
}

/*
void Object::assign(const Expression* value) {
  m_log.warning("Object assignment is unimplemented");
}
*/
