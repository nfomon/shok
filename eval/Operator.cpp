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

int Operator::priority() const {
  if (!m_isComplete) {
    throw EvalError("Cannot query priority of incomplete operator '" + name + "'");
  }
  //if ("COMMA_AND" == name)
  //  return 1;
  if ("OR" == name || "XOR" == name || "XNOR" == name)
    return 2;
  if ("AND" == name)
    return 3;
  if ("EQ" == name || "NE" == name)
    return 4;
  if ("LT" == name || "LE" == name || "GT" == name || "GE" == name)
    return 5;
  if ("USEROP" == name)
    return 6;
  if ("TILDE" == name || "DOUBLETILDE" == name)
    return 7;
  if (isBinary && ("PLUS" == name || "MINUS" == name))
    return 8;
  if ("MULT" == name || "DIV" == name || "MOD" == name)
    return 9;
  if ("POWER" == name)
    return 10;
  if ("NOT" == name)
    return 11;
  if (isUnary && ("PLUS" == name || "MINUS" == name))
    return 12;
  if ("PIPE" == name)
    return 13;
  if ("AMP" == name)
    return 14;
  if ("paren" == name || "bracket" == name)
    return 15;
  throw EvalError("Unknown priority for operator '" + name + "'");
}
