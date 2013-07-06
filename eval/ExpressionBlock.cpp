#include "ExpressionBlock.h"

#include "EvalError.h"

#include <string>
using std::string;

using namespace eval;

void ExpressionBlock::complete() {
  if (children.size() != 1) {
    throw EvalError("Only a single expression is allowed in an ExpressionBlock");
  }
}

void ExpressionBlock::evaluate() {
  // TODO: call the resulting object's ->str()->escape() (*UNIMPL*)
  throw EvalError("Cannot evaluate expression-block: unimplemented");
  //m_str = children.front()->cmdText();
}

string ExpressionBlock::cmdText() const {
  return m_str;
}
