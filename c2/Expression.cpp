// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Expression.h"

#include "Operator.h"

#include "util/Util.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>
using std::auto_ptr;
using std::string;
using std::vector;

using namespace compiler;

Expression::Expression()
  : m_scope(NULL),
    m_infixing(false) {
}

Expression::~Expression() {
  for (stack_iter i = m_stack.begin(); i != m_stack.end(); ++i) {
    delete *i;
  }
}

void Expression::init(Scope& scope) {
  m_scope = &scope;
}

void Expression::attach_atom(const Variable& atom) {
  // TODO
  // For now the Expression has a single Variable atom.  Our type is that the
  // Expression is a descendent of the variable, and adds no new members.
  m_type.reset(new BasicType(atom.type().duplicate(), atom.fullname()));
}

void Expression::attach_preop(const std::string& preop) {
  PrefixOperator op(preop);
  // TODO
}

void Expression::attach_binop(const std::string& binop) {
  InfixOperator op(binop);
  // TODO
}

void Expression::finalize() {
}

std::string Expression::bytecode() const {
  return "<bytecode for " + (m_type ? m_type->print() : "n/a") + ">";
}

const Type& Expression::type() const {
  if (!m_type.get()) {
    throw CompileError("Cannot get type of untyped Expression");
  }
  return *m_type;
}
