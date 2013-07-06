#include "CommandFragment.h"

#include "EvalError.h"

#include <string>
using std::string;

using namespace eval;

void CommandFragment::setup() {
}

void CommandFragment::evaluate() {
}

string CommandFragment::cmdText() const {
  return value;
}
