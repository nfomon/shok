// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Or_h_
#define _Or_h_

#include "ComputeFunc.h"
#include "Rule.h"

#include <memory>
#include <string>

namespace statik {

std::auto_ptr<Rule> OR(const std::string& name);

struct ComputeFunc_Or : public ComputeFunc {
  virtual ~ComputeFunc_Or() {}
  virtual void operator() ();
  virtual std::auto_ptr<ComputeFunc> Clone() {
    return std::auto_ptr<ComputeFunc>(new ComputeFunc_Or());
  }
};

std::auto_ptr<ComputeFunc> MakeComputeFunc_Or();

}

#endif // _Or_h_
