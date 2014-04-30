// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Type.h"

#include "SymbolTable.h"

#include "util/Util.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <memory>
#include <string>
#include <utility>
using std::auto_ptr;
using std::string;

using namespace compiler;

/* RootType */

void RootType::addMember(const string& name, auto_ptr<Type> type) {
  m_members.insert(name, type);
}

const Type* RootType::findMember(const string& name) const {
  symbol_iter s = m_members.find(name);
  if (s != m_members.end()) return s->second;
  return NULL;
}

// It would probably be valid to disallow this, since it's put only once into
// the root scope.
auto_ptr<Type> RootType::duplicate() const {
  RootType* rt = new RootType();
  auto_ptr<Type> art(rt);
  rt->m_members = m_members;
  return art;
}

std::string RootType::defaultValueBytecode() const {
  return "object";
}

/* BasicType */

BasicType::BasicType(auto_ptr<Type> parent, const string& parentName)
  : m_parent(parent),
    m_parentName(parentName) {
}

string BasicType::print() const {
  string s = string("BasicType(") + m_parentName + ")";
  if (m_members.size() > 0) {
    s += " with " + lexical_cast<string>(m_members.size()) + " members";
  }
  return s;
}

void BasicType::addMember(const string& name, auto_ptr<Type> type) {
  m_members.insert(name, type);
}

const Type* BasicType::findMember(const string& name) const {
  symbol_iter s = m_members.find(name);
  if (s != m_members.end()) return s->second;
  return m_parent->findMember(name);
}

auto_ptr<Type> BasicType::duplicate() const {
  BasicType* bt = new BasicType(m_parent->duplicate(), m_parentName);
  auto_ptr<Type> abt(bt);
  bt->m_members = m_members;
  return abt;
}

std::string BasicType::defaultValueBytecode() const {
  std::string bc("(object");
  bc += ")";
  return bc;
}
