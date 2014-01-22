// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "IsVar.h"

#include "EvalError.h"
#include "Identifier.h"

#include <iostream>
#include <memory>
#include <string>
using std::auto_ptr;
using std::cout;
using std::endl;
using std::string;

using namespace eval;

void IsVar::setup() {
  if (children.size() < 1) {
    throw EvalError("IsVar must have >= 1 children");
  }
  auto_ptr<Type> current(NULL);
  bool found = true;
  string missingName;
  size_t i = 0;
  for (; i < children.size(); ++i) {
    Identifier* ident = dynamic_cast<Identifier*>(children.at(i));
    if (!ident) {
      throw EvalError("Children of IsVar " + print() + " must be Identifiers");
    }
    if (!current.get()) {
      missingName = ident->getName();
      Symbol* s = parentScope->getSymbol(ident->getName());
      if (!s) {
        found = false;
        break;
      }
      current = s->type->duplicate();
    } else {
      missingName += "." + ident->getName();
      current = current->getMemberType(ident->getName());
      if (!current.get()) {
        found = false;
        break;
      }
    }
  }

  string msg;
  if (found) {
    msg = "true";
  } else if (i < children.size()-1) {
    msg = "Object " + missingName + " does not exist";
  } else {
    msg = "false";
  }
  cout << "PRINT:" << msg << endl;
}

// As an "instant" Statement, we should never even get to this stage
void IsVar::evaluate() {
  throw EvalError("Cannot evaluate instant statement " + print());
}
