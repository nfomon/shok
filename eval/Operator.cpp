#include "Operator.h"

#include "EvalError.h"

#include <string>
using std::string;

using namespace eval;

void Operator::complete() {
  switch (children.size()) {
    case 0: isCmd = true; break;
    case 1: isUnary = true; break;
    case 2: isBinary = true; break;
    default: throw EvalError("Operator cannot have > 2 children");
  }
  m_isComplete = true;
}

void Operator::evaluate() {
  if (isCmd) return;
  throw EvalError("Operator '" + name + "' not yet supported for evaluation");
}

string Operator::cmdText() const {
  if (!isCmd) {
    throw EvalError("Cannot display cmdText of operator " + name + " that has children");
  }
  if ("PLUS" == name) return "+";
  if ("MINUS" == name) return "-";
  //if ("MULT" == name) return "*";     // these may have special meaning; not
  //if ("DIV" == name) return "/";      // provided by the parser currently
  throw EvalError("cmdText unsupported for operator '" + name + "'");
}
