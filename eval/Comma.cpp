#include "Comma.h"

#include "EvalError.h"

#include <string>
using std::string;

using namespace eval;

void Comma::complete() {
  if (children.size() != 0) {
    throw EvalError("Comma node cannot (yet) have children");
  }
  m_isComplete = true;
}

void Comma::evaluate() {
}

string Comma::cmdText() const {
  return " ";
}
