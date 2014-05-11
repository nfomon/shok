// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Variable.h"

#include "util/Util.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <string>
#include <utility>
using std::string;

using namespace compiler;

Variable::Variable()
  : m_scope(NULL) {
}

void Variable::init(const Scope& scope) {
  m_scope = &scope;
}

void Variable::attach_name(const std::string& name) {
  m_name = name;
  const Type* type = m_scope->find(name);
  if (!type) {
    throw CompileError("Variable " + name + " does not exist in scope at depth " + lexical_cast<string>(m_scope->depth()));
  }
  m_type.reset(type->duplicate().release());
}

void Variable::attach_member(const std::string& member) {
  if (!m_type) {
    throw CompileError("Cannot attach member " + member + " to variable " + m_name + " that is missing a type");
  }
  const Type* type = m_type->findMember(member);
  if (!type) {
    throw CompileError("Variable member " + fullname() + " does not exist in scope at depth " + lexical_cast<string>(m_scope->depth()));
  }
  m_type = type->duplicate();
  m_members.push_back(member);
}

std::string Variable::fullname() const {
  std::string name = m_name;
  for (member_iter i = m_members.begin(); i != m_members.end(); ++i) {
    name += "." + *i;
  }
  return name;
}
