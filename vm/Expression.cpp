// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Expression.h"

#include "Instruction.h"
#include "Instructions.h"
#include "Object.h"
#include "StdLib.h"
#include "VMError.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace vm;

Exec_Exp::Exec_Exp(Context& context)
  : m_context(context),
    m_rootObject(context.findGlobal(StdLib::OBJECT)),
    m_rootFunction(context.findGlobal(StdLib::FUNCTION)) {
  if (!m_rootObject) {
    throw VMError("Failed to find the root object " + StdLib::OBJECT);
  } else if (!m_rootFunction) {
    throw VMError("Failed to find the root function " + StdLib::FUNCTION);
  }
}

auto_ptr<Object> Exec_Exp::operator() (const Variable& var) {
  Object *o = m_context.find(var);
  if (!o) {
    throw VMError("Variable " + var + " not found");
  }
  return auto_ptr<Object>(new Object(*o));
}

auto_ptr<Object> Exec_Exp::operator() (const IntLiteral& lit) {
  Object *o = m_context.find(StdLib::INTEGER);
  if (!o) {
    throw VMError("Failed to find the root integer type " + StdLib::INTEGER);
  }
  auto_ptr<Object> i(new Object(*o));
  i->insertBuiltin(boost::lexical_cast<int>(lit.num));
  return i;
}

auto_ptr<Object> Exec_Exp::operator() (const StringLiteral& lit) {
  Object *o = m_context.find(StdLib::STRING);
  if (!o) {
    throw VMError("Failed to find the root integer type " + StdLib::STRING);
  }
  auto_ptr<Object> i(new Object(*o));
  i->insertBuiltin(lit.str);
  return i;
}

auto_ptr<Object> Exec_Exp::operator() (const MethodCall& methodCall) {
  // Get the source object
  Exec_Exp exec_Exp(m_context);
  auto_ptr<Object> source = boost::apply_visitor(exec_Exp, methodCall.source);

  // Lookup the method
  Object *method = source->find(methodCall.method);
  if (!method) {
    throw VMError("Method " + methodCall.method + " not found on Object");
  }

  // Evaluate the args
  args_vec args;
  for (argexps_iter i = methodCall.argexps.begin();
       i != methodCall.argexps.end(); ++i) {
    args.push_back(boost::apply_visitor(exec_Exp, *i));
  }

  // Call the method
  return method->callFunction(m_context, args);
}

auto_ptr<Object> Exec_Exp::operator() (const ObjectLiteral& object) {
  Exec_Exp exec_Exp(m_context);
  auto_ptr<Object> o(new Object(*m_rootObject));
  for (ObjectLiteral::member_iter i = object.members.begin();
       i != object.members.end(); ++i) {
    auto_ptr<Object> value = boost::apply_visitor(exec_Exp, i->second);
    o->insert(i->first, value);
  }
  return o;
}

auto_ptr<Object> Exec_Exp::operator() (const FunctionLiteral& function) {
  auto_ptr<Object> f(new Object(*m_rootFunction));
  f->insertFunction(auto_ptr<function_vec>(new function_vec(function.body)));
  return f;
}
