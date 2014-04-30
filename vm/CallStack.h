// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _CallStack_h_
#define _CallStack_h_

/* Execution call stack */

#include "StackFrame.h"

#include <boost/ptr_container/ptr_vector.hpp>

#include <memory>
#include <string>

namespace vm {

class CallStack {
public:
  void Call();
  void Return();

private:
  typedef boost::ptr_vector<StackFrame> stack_vec;
  typedef stack_vec::const_iterator stack_iter;

  stack_vec m_stack;
};

}

#endif // _CallStack_h_
