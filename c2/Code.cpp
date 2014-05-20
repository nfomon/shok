// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Code.h"

#include "util/Util.h"

#include <string>
#include <utility>
using std::string;

using namespace compiler;

void Call::init(const Scope& scope) {
  m_scope = &scope;
  m_bytecode = " (call ";
}

void Call::attach_source(const Variable& var) {
  // TODO validate that var is callable
  m_bytecode += var.bytename();
}

void Call::attach_arg(const Expression& arg) {
  // TODO validate that var's next arg accepts this arg's type
  m_bytecode += arg.bytecode();
}

string Call::bytecode() const {
  return m_bytecode + ")";
}
