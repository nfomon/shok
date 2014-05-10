// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Scope.h"

#include "util/Util.h"

#include <memory>
#include <string>
#include <utility>
using std::auto_ptr;
using std::string;

using namespace compiler;

/* Scope */

Scope::Scope(const Scope* parent)
  : m_parent(parent),
    m_root(parent ? parent->root() : *this),
    m_depth(parent ? (parent->depth() + 1) : 0) {
}

void Scope::insert(const std::string& name, std::auto_ptr<Type> type) {
  m_locals.insert(name, type);
}

const Type* Scope::find(const string& name) const {
  symbol_iter s = m_locals.find(name);
  if (s != m_locals.end()) return s->second;
  if (m_parent) return m_parent->find(name);
  return NULL;
}

const Type* Scope::findLocal(const string& name) const {
  symbol_iter s = m_locals.find(name);
  if (s != m_locals.end()) return s->second;
  return NULL;
}

const Type* Scope::findRoot(const string& name) const {
  return m_root.find(name);
}

/* ObjectScope */

ObjectScope::ObjectScope(const Object& object, const Scope& parent)
  : Scope(&parent),
    m_object(object) {
}

const Type* ObjectScope::find(const string& name) const {
  const Type* t = m_object.type().findMember(name);
  if (t) return t;
  return m_parent->find(name);
}

const Type* ObjectScope::findLocal(const string& name) const {
  return m_object.type().findMember(name);
}
