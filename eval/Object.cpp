// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

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
    m_type(type),
    m_isAbstract(false) {
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

bool Object::takesArgs(const type_list& args) const {
  // TODO
  /*
  for (signature_iter i = m_signatures.begin(); i != m_signatures.end(); ++i) {
    if (i->areArgsCompatible(args)) {
      return true;
    }
  }
  */
  return false;
}

auto_ptr<Type> Object::getPossibleReturnTypes(const type_list& args) const {
  if (!takesArgs(args)) {
    throw EvalError("Function " + print() + " does not take these args");
  }
  auto_ptr<Type> returnTypes(NULL);
  // TODO
  /*
  for (signature_iter i = m_signatures.begin(); i != m_signatures.end(); ++i) {
    if (i->areArgsCompatible(args)) {
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

auto_ptr<Object> Object::call(const object_list& args) const {
  // TODO
  return auto_ptr<Object>(NULL);
}
