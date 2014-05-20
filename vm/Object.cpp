// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Object.h"

#include "Instruction.h"
#include "Instructions.h"
#include "VMError.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

// debug
#include <iostream>
using std::cout;
using std::endl;

using namespace vm;

Object::Object() {
}

Object::Object(const Object& rhs) {
  if (rhs.m_function.get()) {
    m_function.reset(new function_vec(*rhs.m_function.get()));
  }
}

Object& Object::operator=(const Object& rhs) {
  if (rhs.m_function.get()) {
    m_function.reset(new function_vec(*rhs.m_function.get()));
  }
  return *this;
}

Object::~Object() {
  cout << "Object deleted" << endl;
}

const Object* Object::find(const std::string& name) const {
  symbol_iter s = m_members.find(name);
  if (s != m_members.end()) {
    return s->second;
  }
  return NULL;
}

Object* Object::find(const std::string& name) {
  symbol_mod_iter s = m_members.find(name);
  if (s != m_members.end()) {
    return s->second;
  }
  return NULL;
}

void Object::insert(const string& name, auto_ptr<Object> value) {
  symbol_iter m = m_members.find(name);
  if(m != m_members.end()) {
    throw VMError("Cannot insert member " + name + "; already exists");
  }
  m_members.insert(name, value);  // TODO use iterator
}

void Object::assign(const string& name, auto_ptr<Object> value) {
  symbol_iter m = m_members.find(name);
  if(m_members.end() == m) {
    throw VMError("Cannot assign to member " + name + "; no such member found on object");
  }
  m_members.insert(name, value);  // TODO use iterator
}

auto_ptr<Object> Object::callFunction(Context& context,
                                      const args_vec& args) const {
  if (!m_function.get()) {
    throw VMError("Cannot call non-function Object");
  }
  cout << "Calling function" << endl;
  const function_vec& function = *m_function.get();
  context.addFrame();
  // TODO assign named args
  // TODO run function body
  Exec_Instruction exec_Instruction(context);
  for (function_iter i = function.begin(); i != function.end(); ++i) {
    boost::apply_visitor(exec_Instruction, *i);
  }
  context.removeFrame();
  // TODO get return value (from context?)
  return auto_ptr<Object>();
}

void Object::insertFunction(auto_ptr<function_vec> function) {
  if (m_function.get()) {
    throw VMError("Cannot insert function into Object that is already a function");
  }
  m_function = function;
}
