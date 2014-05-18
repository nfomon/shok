// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Call.h"

#include "VMError.h"

#include "util/Util.h"

#include <boost/ptr_container/ptr_vector.hpp>

#include <string>
#include <vector>
using std::auto_ptr;
using std::string;
using std::vector;

// debug
#include <iostream>
using std::cout;
using std::endl;

using namespace vm;

void Exec_Call::operator() (const Call& call, qi::unused_type, qi::unused_type) const {
  cout << "Call" << endl;
  Exec_Exp exec_Exp(m_symbols);
  auto_ptr<Object> function = boost::apply_visitor(exec_Exp, call.function);
  boost::ptr_vector<Object> args;
  for (vector<Expression>::const_iterator i = call.args.begin();
       i != call.args.end(); ++i) {
    args.push_back(boost::apply_visitor(exec_Exp, call.function));
  }
  // Actually call the function
  // Lookup the signature that best matches our arguments
  // Create a stack frame for the call
  //std::vector<StackFrame*> callstack; // ??
  // Give the frame our args
  // etc.
}
