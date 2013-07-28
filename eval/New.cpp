// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "New.h"

#include "EvalError.h"
#include "NewInit.h"

#include <string>
using std::string;

using namespace eval;

void New::setup() {
  // Children are inits
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    NewInit* init = dynamic_cast<NewInit*>(*i);
    if (!init) {
      throw EvalError("New statement's children must all be NewInit nodes");
    }
  }
}

void New::analyze() {
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    NewInit* init = dynamic_cast<NewInit*>(*i);
    init->prepare();
  }
}

// Children will have already been evaluated (commit their changesets)
void New::evaluate() {
}
