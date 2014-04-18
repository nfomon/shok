// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "ProcCall.h"

#include "Expression.h"
#include "CompileError.h"
#include "Function.h"

#include <memory>
#include <string>
#include <vector>
using std::auto_ptr;
using std::string;
using std::vector;

using namespace compiler;

void ProcCall::setup() {
  if (children.size() < 1) {
    throw CompileError("ProcCall must have >= 1 children");
  }
  Variable* var = dynamic_cast<Variable*>(children.at(0));
  if (!var) {
    throw CompileError("ProcCall first child must be a Variable");
  }
  m_object = &var->getObject();
  /*
  if (!m_object->isFunction()) {
    throw CompileError("ProcCall cannot call a non-function");
  }
  */
  for (child_iter i = children.begin()+1; i != children.end(); ++i) {
    Expression* exp = dynamic_cast<Expression*>(*i);
    if (!exp) {
      throw CompileError("ProcCall params must be Expressions");
    }
    m_paramexps.push_back(exp);
    m_paramtypes.push_back(&exp->type());
  }
  // Verify that the function has a signature for these param types
  /*
  if (!m_object->takesArgs(m_paramtypes)) {
    throw CompileError("Function " + m_object->print() + " does not accept the provided parameters");
  }
  */
}

void ProcCall::compile() {
  // The expressions of m_paramexps have all been compiled.  Call the function
  // on their resulting objects.  The object being called gets ownership of the
  // params.
  param_vec params;
  for (exp_iter i = m_paramexps.begin(); i != m_paramexps.end(); ++i) {
    // TODO: ask the object for the argument names it wants to use (on the
    // specific signature we're calling)
    params.push_back((*i)->getObject("(some arg)").release());
  }
  //auto_ptr<Object> ret = m_object->call(params);
}

void ProcCall::computeType() {
  // ProcCall type is the return type of the function
  //m_type = m_object->getPossibleReturnTypes(m_paramtypes);
}
