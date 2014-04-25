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
  if (m_scope->findLocal(name) != NULL) {
    throw CompileError("Variable " + name + " already exists in scope at depth " + boost::lexical_cast<string>(m_scope->depth()));
  }
}

void NewInit::attach_type(const std::string& type) {
  m_type.reset(new RootType());
}

void NewInit::attach_exp(const std::string& exp) {
  // TODO
  //m_exp = Expression();
}

void NewInit::finalize() {
  if (!m_type.get()) {
    // TODO make a BasicType(stdlib::object)
    m_type.reset(new RootType());
  }
  // TODO also handle !m_exp
  // TODO set m_exp if unset, so that bytecode() can use both m_type and m_exp
  m_scope->insert(m_name, std::auto_ptr<Type>(m_type->duplicate()));
}

std::string NewInit::bytecode() const {
  // if !m_hasExp, then the initial value comes from the type
  return "<newinit>";
}
