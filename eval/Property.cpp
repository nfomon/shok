// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "Property.h"

#include "EvalError.h"
#include "Variable.h"

#include <string>
using std::string;

using namespace eval;

void Property::setup() {
  if (children.size() != 2) {
    throw EvalError("Property node requires 2 children");
  }
  // Left child is a Variable
  Variable* objVar = dynamic_cast<Variable*>(children.at(0));
  if (!objVar) {
    throw EvalError("Property's first child must be a Variable");
  }
  m_objectName = objVar->getVariableName();
  // Right child is either a Property or a Variable
  m_subProperty = dynamic_cast<Property*>(children.at(1));
  Variable* var = dynamic_cast<Variable*>(children.at(1));
  if ((m_subProperty && var) || (!m_subProperty && !var)) {
    throw EvalError("Property's second child must be either a Property or a Variable");
  }
  if (var) {
    m_propertyName = var->getVariableName();
    if ("" == m_propertyName) {
      throw EvalError("Terminal Property variable name must not be blank");
    }
  }
}

std::string Property::getPropertyName() const {
  if (isTerminal()) {
  } else if ("" == m_propertyName) {
    throw EvalError("Property name for " + print() + " is unexpectedly blank");
  }
  return m_propertyName;
}

Property* Property::getSubProperty() const {
  if (isTerminal()) {
    throw EvalError("Cannot get subproperty of terminal Property");
  }
  return m_subProperty;
}

// Nothing to do
void Property::evaluate() {
}
