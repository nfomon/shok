// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "IntLiteral.h"

#include "StdLib.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <string>
using std::string;

using namespace compiler;

IntLiteral::IntLiteral()
  : m_scope(NULL) {
}

void IntLiteral::init(const Scope& scope) {
  m_scope = &scope;
  const Type* type = m_scope->find(StdLib::INTEGER);
  if (!type) {
    throw CompileError("Object for literal int does not exist in scope at depth " + lexical_cast<string>(m_scope->depth()));
  }
  m_type.reset(type->duplicate().release());
}

void IntLiteral::attach_text(const std::string& text) {
  m_text = text;
}

const Type& IntLiteral::type() const {
  if (!m_type) {
    throw CompileError("IntLiteral has no type");
  }
  return *m_type;
}

std::string IntLiteral::bytecode() const {
  return "(int " + m_text + ")";
}
