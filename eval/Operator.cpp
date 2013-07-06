#include "Operator.h"

#include "EvalError.h"

#include <string>
using std::string;

using namespace eval;

void Operator::complete() {
  switch (children.size()) {
    case 1: isUnary = true; break;
    case 2: isBinary = true; break;
    default: throw EvalError("Operator cannot have > 2 children");
  }
}

void Operator::analyze() {
  // For overloadable operators, see if the operand has implemented a method
  // for this operator.
  // If it's not overloadable, what are we doing here?  We probably should have
  // been subclassed :)
  // Note that some operators require specific types of their operands, or
  // other special evaluations (e.g. ~ performs a ->str() on its operands).
  //if (isOverloadable) {
    // Unary => Prefix operator, for now
  //} else {
  //}

  // Our type is the type of the result of the operation
}

void Operator::evaluate() {
  throw EvalError("Operator '" + name + "' not yet supported for evaluation");
}

int Operator::priority() const {
  if (!m_isComplete) {
    throw EvalError("Cannot query priority of incomplete operator '" + name + "'");
  }
  //if ("COMMA_AND" == name)
  //  return 0;
  if ("DOT" == name)
    return 1;
  if ("OR" == name || "NOR" == name || "XOR" == name || "XNOR" == name)
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
  if ("STAR" == name || "SLASH" == name || "PERCENT" == name)
    return 9;
  if ("CARAT" == name)
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
