// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "CallStack.h"
#include "Object.h"

#include "VMError.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace vm;

void CallStack::Call() {
  m_stack.push_back(StackFrame());
}

void CallStack::Return() {
  if (m_stack.empty()) {
    throw VMError("Cannot return from outermost stack frame");
  }
  m_stack.pop_back();
}
