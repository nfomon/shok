#include "ExpressionBlock.h"

#include "EvalError.h"

#include <string>
using std::string;

using namespace eval;

void ExpressionBlock::complete() {
  if (children.size() != 1) {
    throw EvalError("Only a single expression is allowed in an ExpressionBlock");
  }
  children.front()->complete();
  m_isComplete = true;
}

void ExpressionBlock::evaluate() {
  children.front()->evaluate();
  // DANGER: TODO: call the resulting object's ->str()->escape() (*UNIMPL*)
  log.warning("String-escapes from exp blocks are unimplemented");
  m_str = children.front()->cmdText();
}

string ExpressionBlock::cmdText() const {
  return m_str;
}
