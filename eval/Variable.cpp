#include "Variable.h"

#include "EvalError.h"

#include <string>
using std::string;

using namespace eval;

void Variable::complete() {
  if (children.size() != 0) {
    throw EvalError("Variable node cannot have children");
  }
}

void Variable::analyze() {
  //type = BasicType(this);
}

void Variable::evaluate() {
}
