// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Args.h"

#include "Arg.h"
#include "Common.h"
#include "EvalError.h"

#include <string>
#include <vector>
using std::string;
using std::vector;

using namespace eval;

void Args::setup() {
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    Arg* arg = dynamic_cast<Arg*>(*i);
    if (!arg) {
      throw EvalError("Args " + print() + " found non-Arg child " + (*i)->print());
    }
    m_args.push_back(arg);
  }
}

// Nothing to do
void Args::evaluate() {
}

const arg_vec& Args::getArgs() const {
  if (!isSetup) {
    throw EvalError("Cannot get args from Args " + print() + " before it is setup");
  }
  return m_args;
}
