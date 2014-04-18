// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Variable.h"

#include "CompileError.h"
#include "Identifier.h"
#include "Type.h"

#include <memory>
#include <string>
#include <vector>
using std::auto_ptr;
using std::string;
using std::vector;

using namespace compiler;

void Variable::setup() {
  if (children.size() < 1) {
    throw CompileError("Variable node must have >= 1 children");
  }
  auto_ptr<Type> memberType;
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    Identifier* ident = dynamic_cast<Identifier*>(*i);
    if (!ident) {
      throw CompileError("Variable children must all be Identifiers");
    }
    if (!m_symbol) {
      m_varname = ident->getName();
      m_fullname = m_varname;
      m_symbol = parentScope->getSymbol(m_varname);
      if (!m_symbol) {
        throw CompileError("Object " + m_varname + " does not exist");
      }
      memberType = m_symbol->type->duplicate();
    } else {
      string member = ident->getName();
      m_fullname += "." + member;
      memberType = memberType->getMemberType(member);
      if (!memberType.get()) {
        throw CompileError("Object member " + m_fullname + " does not exist");
      }
      m_select.push_back(member);
    }
  }
  computeType();
}

Object& Variable::getObject() const {
  if (!m_symbol) {
    throw CompileError("Cannot retrieve Symbol of deficient Variable " + print());
  } else if (!m_symbol->object.get()) {
    throw CompileError("Cannot retrieve Object of deficient Variable " + print());
  }
  return *m_symbol->object.get();
}

void Variable::computeType() {
  if (!m_symbol) {
    throw CompileError("Failed to find Symbol behind Variable " + print());
  }
  //m_type.reset(new BasicType(log, *m_symbol, m_select));    // TODO
  m_type.reset(new BasicType(log, *m_symbol));
}
