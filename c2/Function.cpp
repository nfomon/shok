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

void Function::init_args() {
  if (m_type) {
    throw CompileError("Cannot init args of Function that already has a type");
  }
  m_type.reset(new ArgsType(*m_froot));
}

void Function::attach_arg(const std::string& name, const Expression& exp) {
  ArgsType* argsType = dynamic_cast<ArgsType*>(m_type.get());
  if (!argsType) {
    throw CompileError("Cannot attach arg " + name + " to Function with uninitialized args");
  }
  argsType->addArg(name, exp.type().duplicate());
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

const Type& Function::type() const {
  if (!m_type.get()) {
    throw CompileError("Function " + bytecode() + " does not have a type");
  }
  return *m_type;
}

string Function::bytecode() const {
  return " (function" + m_bytecode + m_scope->bytecode() + ")";
}
