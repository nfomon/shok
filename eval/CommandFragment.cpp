#include "CommandFragment.h"

#include "EvalError.h"

#include <string>
using std::string;

using namespace eval;

void CommandFragment::complete() {
}

void CommandFragment::evaluate() {
}

string CommandFragment::cmdText() const {
  return value;
}
