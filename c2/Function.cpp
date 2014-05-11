// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Function.h"

#include "Expression.h"

#include "util/Util.h"

#include <memory>
#include <string>
#include <utility>
using std::auto_ptr;
using std::string;

using namespace compiler;

void Function::init(const Scope& scope) {
  m_scope.reset(new FunctionScope(scope, *this));
  m_froot = m_scope->findRoot("@");
  if (!m_froot) {
    throw CompileError("Cannot initialize Function; @ is missing");
  }
}

void Function::attach_arg(const std::string& name, const Expression& exp) {
  // TODO dropping the name on the floor, for now.  Where does it belong?
  // TODO validate the arg (e.g. name collision?)
  m_args.push_back(exp.type().duplicate());
}

void Function::finalize_args() {
  if (m_args.size() > 0) {
    m_type.reset(new ArgsType(*m_froot, m_args));
  }
}

void Function::attach_returns(const Expression& returns) {
  if (m_type.get()) {
    m_type.reset(new AndType(m_type->duplicate(),
        auto_ptr<Type>(new ReturnsType(*m_froot, returns.type().duplicate()))));
  } else {
    m_type.reset(new ReturnsType(*m_froot, returns.type().duplicate()));
  }
}

void Function::attach_body(const string& code) {
  m_bytecode += code;
}

FunctionScope& Function::scope() const {
  return *m_scope;
}

string Function::bytecode() const {
  return " (function" + m_bytecode + ")";
}
