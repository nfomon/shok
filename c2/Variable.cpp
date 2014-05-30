// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Variable.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <string>
#include <utility>
using std::string;

using namespace compiler;

Variable::Variable()
  : m_scope(NULL),
    m_varScope(NULL) {
}

void Variable::init(const Scope& scope) {
  m_scope = &scope;
}

void Variable::attach_name(const std::string& name) {
  m_name = name;
  m_varScope = m_scope->findScope(name);
  if (!m_varScope) {
    throw CompileError("Variable " + name + " does not exist in scope at depth " + lexical_cast<string>(m_scope->depth()));
  }
  const Type* type = m_varScope->findLocal(name);
  if (!type) {
    throw CompileError("Lookup failure for variable " + name + " in scope at depth " + lexical_cast<string>(m_varScope->depth()));
  }
  m_type.reset(type->duplicate().release());
}

void Variable::attach_member(const std::string& member) {
  if (!m_type) {
    throw CompileError("Cannot attach member " + member + " to variable " + m_name + " that is missing a type");
  }
  const Type* type = m_type->findMember(member);
  if (!type) {
    throw CompileError("Variable member " + fullname() + " does not exist");
  }
  m_type = type->duplicate();
  m_members.push_back(member);
}

const Type& Variable::type() const {
  if (!m_type) {
    throw CompileError("Variable " + fullname() + " has no type");
  }
  return *m_type;
}

std::string Variable::fullname() const {
  std::string name = m_name;
  for (member_iter i = m_members.begin(); i != m_members.end(); ++i) {
    name += "." + *i;
  }
  return name;
}

std::string Variable::bytename() const {
  std::string name = m_varScope->bytename(m_name);
  for (member_iter i = m_members.begin(); i != m_members.end(); ++i) {
    name += "." + *i;
  }
  return name;
}
