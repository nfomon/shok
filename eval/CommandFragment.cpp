// Copyright (C) 2013 Michael Biggs.  See the COPYRIGHT file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

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
