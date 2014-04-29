// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "NewInit.h"

#include "util/Util.h"

#include <boost/lexical_cast.hpp>

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

void NewInit::attach_name(const std::string& name) {
  m_name = name;
  if (m_scope->findLocal(name)) {
    throw CompileError("Variable " + name + " already exists in scope at depth " + boost::lexical_cast<string>(m_scope->depth()));
  }
}

void NewInit::attach_type(const Type& type) {
  m_type.reset(type.duplicate().release());
}

void NewInit::attach_exp(const Expression& exp) {
  if (!m_type.get()) {
    m_type.reset(exp.type().duplicate().release());
  }
  m_bytecode = exp.bytecode();
}

void NewInit::finalize() {
  if (!m_type.get()) {
    const Type* object = m_scope->findRoot("object");
    if (!object) {
      throw CompileError("NewInit finalize failed to find the root object type");
    }
    m_type.reset(new BasicType(object->duplicate(), "object"));
  } else {
    // TODO if we have a type but we did not attach_exp, need the bytecode (+
    // bytecode value-identifier) from constructing the type's default value
  }
  m_scope->insert(m_name, m_type->duplicate());
  m_bytecode += "new " + m_name;    // TODO + bytecode value-identifier
}

std::string NewInit::bytecode() const {
  // if !m_hasExp, then the initial value comes from the type
  return "<newinit>";
}
