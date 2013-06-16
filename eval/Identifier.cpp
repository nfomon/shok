#include "Identifier.h"

#include "EvalError.h"

#include <string>
using std::string;

using namespace eval;

void Identifier::complete() {
  if (children.size() != 0) {
    throw EvalError("Identifier node cannot have children");
  }
  m_isComplete = true;
}

void Identifier::evaluate() {
}

string Identifier::cmdText() const {
  return value;
}
