// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Expression.h"

#include "VMError.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace vm;

auto_ptr<Object> Exec_Exp::operator() (const string& var) const {
  symbol_iter s = m_symbols.find(var);
  if (m_symbols.end() == s) {
    throw VMError("Variable " + var + " not found");
  }
  return auto_ptr<Object>(new Object(*s->second));
}

auto_ptr<Object> Exec_Exp::operator() (const MethodCall& methodCall) const {
  // I want to apply these in the context of a symbol table etc. (m_symbols)
  // And I want them to return an auto_ptr<Object>.
  // For methodCall, we find the source object, push a function context for
  // the method on our call stack, give it the args, set our stack pointer
  // and return pointer, and finally set the program counter...
  // TODO

  // 1. Get source
  Exec_Exp exec_Exp(m_symbols);
  auto_ptr<Object> source = boost::apply_visitor(exec_Exp, methodCall.source);

  // 2. Construct call-stack frame for the method body, retrieved from
  // source.methodName

  // 3. Evaluate the args into stack-frame locals
  /*
  for (MethodCall::args_iter i = methodCall.args.begin();
       i != methodCall.args.end(); ++i) {
    auto_ptr<Object> arg = boost::apply_visitor(exec_Exp, *i);
  }
  */

  // 4. Execute the stack-frame (??)
  return auto_ptr<Object>(new Object());    // TODO
}

auto_ptr<Object> Exec_Exp::operator() (const ObjectLiteral& object) const {
  // TODO
  return auto_ptr<Object>();
}
