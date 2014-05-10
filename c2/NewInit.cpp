// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "NewInit.h"

#include "util/Util.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <string>
#include <utility>
using std::string;

using namespace compiler;

NewInit::NewInit()
  : m_scope(NULL) {
}

void NewInit::init(Scope& scope) {
  m_scope = &scope;
}

void NewInit::attach_name(const string& name) {
  m_name = name;
  if (m_scope->findLocal(name)) {
    throw CompileError("Variable " + name + " already exists in scope at depth " + lexical_cast<string>(m_scope->depth()));
  }
}

void NewInit::attach_type(const Type& type) {
  m_type.reset(type.duplicate().release());
}

void NewInit::attach_exp(const Expression& exp) {
  m_exp.reset(new Expression(exp));
  if (!m_type.get()) {
    m_type.reset(exp.type().duplicate().release());
  }
  m_bytecode = " " + m_exp->bytecode();
}

void NewInit::finalize() {
  if (m_type.get()) {
    if (!m_exp.get()) {
      m_bytecode = " " + m_type->defaultValueBytecode();
    }
  } else {
    const Type* object = m_scope->findRoot("object");
    if (!object) {
      throw CompileError("NewInit finalize failed to find the root object type");
    }
    m_type.reset(new BasicType(object->duplicate(), "object"));
  }
  // This type dup might be unnecessary
  m_scope->insert(m_name, m_type->duplicate());
}

string NewInit::bytecode_asNew() const {
  return "(new " + m_name + m_bytecode + ")";
}

string NewInit::bytecode_asMember() const {
  return "(member " + m_name + m_bytecode + ")";
}
