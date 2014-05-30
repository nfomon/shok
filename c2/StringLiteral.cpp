// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "StringLiteral.h"

#include "StdLib.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <string>
using std::string;

using namespace compiler;

StringLiteral::StringLiteral()
  : m_scope(NULL) {
}

void StringLiteral::init(const Scope& scope) {
  m_scope = &scope;
  const Type* type = m_scope->find(StdLib::STRING);
  if (!type) {
    throw CompileError("Object for literal string does not exist in scope at depth " + lexical_cast<string>(m_scope->depth()));
  }
  m_type.reset(type->duplicate().release());
}

void StringLiteral::attach_text(const std::string& text) {
  m_text = text;
}

const Type& StringLiteral::type() const {
  if (!m_type) {
    throw CompileError("StringLiteral has no type");
  }
  return *m_type;
}

std::string StringLiteral::bytecode() const {
  return "(str " + m_text + ")";
}
