// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

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
