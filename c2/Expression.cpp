// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Expression.h"

#include "Operator.h"

#include "util/Util.h"

#include <string>
#include <utility>
#include <vector>
using std::string;
using std::vector;

using namespace compiler;

Expression::Expression()
  : m_infixing(false) {
}

void Expression::init(Log& log) {
  m_log = &log;
}

void Expression::attach_atom(const std::string& atom) {
  // TODO
}

void Expression::attach_preop(const std::string& preop) {
  PrefixOperator op(*m_log, preop);
  // TODO
}

void Expression::attach_binop(const std::string& binop) {
  InfixOperator op(*m_log, binop);
  // TODO
}

void Expression::finalize() {
}

std::string Expression::bytecode() const {
  return "<bytecode>";
}
