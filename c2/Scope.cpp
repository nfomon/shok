// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Scope.h"

#include "Function.h"
#include "Object.h"

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
    m_root(parent ? &parent->root() : this),
    m_depth(parent ? (parent->depth() + 1) : 0) {
}

void Scope::reParent(const Scope& newParent) {
  m_parent = &newParent;
  m_root = &newParent.root();
  m_depth = newParent.depth() + 1;
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
  return m_root->find(name);
}

string Scope::bytecode() const {
  string s;
  for (symbol_rev_iter i = m_locals.rbegin(); i != m_locals.rend(); ++i) {
    s += " (del " + i->first + ")";
  }
  return s;
}

/* FunctionScope */

FunctionScope::FunctionScope(const Scope& parent, const Function& function)
  : Scope(&parent),
    m_function(function) {
}

const Type* FunctionScope::find(const string& name) const {
  const Type* t = m_function.type().findMember(name);
  if (t) return t;
  return m_parent->find(name);
}

const Type* FunctionScope::findLocal(const string& name) const {
  return m_function.type().findMember(name);
}

/* ObjectScope */

ObjectScope::ObjectScope(const Scope& parent, const Object& object)
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
