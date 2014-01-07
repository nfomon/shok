// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "IsVar.h"

#include "EvalError.h"
#include "Identifier.h"

#include <iostream>
#include <string>
using std::cout;
using std::endl;
using std::string;

using namespace eval;

void IsVar::setup() {
  if (children.size() < 1) {
    throw EvalError("IsVar must have >= 1 children");
  }
  Object* current = NULL;
  bool found = true;
  string missingName;
  size_t i = 0;
  for (; i < children.size(); ++i) {
    Identifier* ident = dynamic_cast<Identifier*>(children.at(i));
    if (!ident) {
      throw EvalError("Children of IsVar " + print() + " must be Identifiers");
    }
    if (!current) {
      missingName = ident->getName();
      current = parentScope->getObject(ident->getName());
      if (!current) {
        found = false;
        break;
      }
    } else {
      missingName += "." + ident->getName();
      current = current->getMember(ident->getName());
      if (!current) {
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
