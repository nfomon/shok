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

void Call::exec(Context& context) const {
  cout << "Call" << endl;
  Exec_Exp exec_Exp(context);
  auto_ptr<Object> function_obj = boost::apply_visitor(exec_Exp, function);
  boost::ptr_vector<Object> arg_objs;
  for (vector<Expression>::const_iterator i = args.begin();
       i != args.end(); ++i) {
    arg_objs.push_back(boost::apply_visitor(exec_Exp, *i));
  }
  // Actually call the function
  // Lookup the signature that best matches our arguments
  // Create a stack frame for the call
  context.addFrame();
  // Give the frame our args
  // etc.
  context.removeFrame();    // TODO hacky cleanup for now :)
}
