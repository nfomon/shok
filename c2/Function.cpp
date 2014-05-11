// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Function.h"

#include "util/Util.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <string>
#include <utility>
using std::string;

using namespace compiler;

void Function::init(const Scope& scope) {
  m_scope.reset(new FunctionScope(scope, *this));
  m_type.reset(new BasicType(m_scope->findRoot("@")->duplicate(), "@"));
}

void Function::attach_arg(const Arg& arg) {
}

void Function::attach_returns(const Returns& returns) {
}

Scope& Function::scope() const {
  return *dynamic_cast<Scope*>(m_scope.get());
}

string Function::bytecode() const {
  return "(function" + m_bytecode + ")";
}
