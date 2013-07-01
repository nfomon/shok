#include "CommandFragment.h"

#include "EvalError.h"

#include <string>
using std::string;

using namespace eval;

void CommandFragment::complete() {
  m_isComplete = true;
}

void CommandFragment::evaluate() {
}

string CommandFragment::cmdText() const {
  return value;
}
