// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Object.h"

#include "CompileError.h"

#include <memory>
#include <string>
#include <utility>
using std::auto_ptr;
using std::make_pair;
using std::string;

using namespace compiler;

Object::Object(Log& log, const string& name)
  : m_log(log),
    m_name(name),
    m_isAbstract(false),
    m_isConst(false),
    m_isConstructed(false),
    m_isDestructed(false) {
}

Object::~Object() {
  m_log.debug("Object " + print() + " is being destroyed");
  if (!m_isDestructed) {
    m_log.warning("Destroying non-destructed Object " + print());
  }
  for (member_iter i = m_members.begin(); i != m_members.end(); ++i) {
    delete i->second;
  }
  for (method_iter i = m_methods.begin(); i != m_methods.end(); ++i) {
    delete *i;
  }
}

Object* Object::getMember(const string& name) const {
  member_iter i = m_members.find(name);
  if (i != m_members.end()) return i->second;
  return NULL;
}

void Object::newMember(const string& name, auto_ptr<Object> object) {
  if (getMember(name)) {
    throw CompileError("Cannot add new member " + name + " to Object " + print() + "; member already exists");
  }
  m_members.insert(make_pair(name, object.release()));
}

void Object::newMethod(const arg_vec* args,
                       auto_ptr<Type> returnType,
                       auto_ptr<Block> body) {
  // TODO: verify that the method does not conflict or overlap with another
  m_methods.push_back(new Method(args, returnType, body));
}

/*
auto_ptr<Object> Object::call(const param_vec& params) const {
  // TODO
  // ...
  // We own the params, so make sure we delete them!
  for (param_iter i = params.begin(); i != params.end(); ++i) {
    delete *i;
  }
  return auto_ptr<Object>(NULL);
*
  try {
  } catch (...) {
  }
*
}
*/

void Object::construct() {
  if (m_isConstructed) {
    throw CompileError("Cannot construct already-constructed Object " + print());
  } else if (m_isDestructed) {
    throw CompileError("Cannot construct already-destructed Object " + print());
  }
  m_log.info("Object " + print() + ": constructor called");
  m_log.debug("Constructing children of " + print());
  for (member_iter i = m_members.begin(); i != m_members.end(); ++i) {
    i->second->construct();
  }
  m_isConstructed = true;
}

void Object::destruct() {
  if (!m_isConstructed) {
    throw CompileError("Cannot destruct non-constructed Object " + print());
  } else if (m_isDestructed) {
    throw CompileError("Cannot destruct already-destructed Object " + print());
  }
  m_log.info("Object " + print() + ": destructor called");
  m_log.debug("Destroying children of " + print());
  for (member_iter i = m_members.begin(); i != m_members.end(); ++i) {
    i->second->destruct();
    delete i->second;
  }
  m_members.clear();
  m_isDestructed = true;
}

auto_ptr<Object> Object::clone(const string& newName) const {
  m_log.info("Cloning object " + print() + " into " + newName);
  auto_ptr<Object> o(new Object(m_log, newName));
  for (member_iter i = m_members.begin(); i != m_members.end(); ++i) {
    o->m_members.insert(make_pair(i->first, i->second->clone(i->first).release()));
  }
  // TODO
  /*
  for (method_iter i = m_methods.begin(); i != m_methods.end(); ++i) {
  }
  */
  o->m_isAbstract = m_isAbstract;
  o->m_isConst = m_isConst;
  return o;
}
