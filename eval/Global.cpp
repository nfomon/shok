#include "Global.h"

#include "EvalError.h"
#include "Variable.h"

#include <string>
#include <vector>
using std::string;
using std::vector;

using namespace eval;

Global::Global() {
}

Global::~Global() {
}

bool Global::hasVariable(Variable* v) const {
  return m_variables.find(v) != m_variables.end();
}

void Global::addVariable(Variable* v) {
  if (hasVariable(v)) {
    throw EvalError("Variable " + v->print() + " already exists in global scope");
  }
  m_variables.insert(v);
}
