// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "IsVar.h"

#include "EvalError.h"
#include "Property.h"
#include "Variable.h"

#include <iostream>
#include <string>
using std::cout;
using std::endl;
using std::string;

using namespace eval;

void IsVar::setup() {
  if (children.size() != 1) {
    throw EvalError("IsVar must have a single child");
  }
  Variable* var = dynamic_cast<Variable*>(children.at(0));
  Property* prop = dynamic_cast<Property*>(children.at(0));
  if ((var && prop) || (!var && !prop)) {
    throw EvalError("IsVar must have a single Variable or Property as child");
  }
  if (var) {
    bool isvar = parentScope->getObject(var->getVariableName()) != NULL;
    cout << "PRINT:" << (isvar ? "true" : "false") << endl;
    return;
  } else if (prop) {
    Object* obj = parentScope->getObject(prop->getObjectName());
    if (!obj) {
      cout << "PRINT:Object " << prop->getObjectName() << " does not exist" << endl;
      return;
    }
    while (!prop->isTerminal()) {
      prop = prop->getSubProperty();
      obj = obj->getMember(prop->getObjectName());
      if (!obj) {
        cout << "PRINT:Object " << prop->getObjectName() << " does not exist" << endl;
        return;
      }
    }
    bool isvar = obj->getMember(prop->getPropertyName()) != NULL;
    cout << "PRINT:" << (isvar ? "true" : "false") << endl;
    return;
  }
  throw EvalError("IsVar must have either a Variable or Property as child");
}

// Nothing to do here
void IsVar::evaluate() {
}
