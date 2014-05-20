// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Call.h"

#include "Instructions.h"
#include "Object.h"
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
  Exec_Exp exec_Exp(context);
  auto_ptr<Object> function_obj = boost::apply_visitor(exec_Exp, function);
  args_vec args;
  for (vector<Expression>::const_iterator i = argexps.begin();
       i != argexps.end(); ++i) {
    args.push_back(boost::apply_visitor(exec_Exp, *i));
  }
  // Call the function, discarding its return value
  (void) function_obj->callFunction(context, args);
}
