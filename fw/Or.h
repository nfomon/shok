// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Or_h_
#define _Or_h_

#include "ComputeFunc.h"
#include "Rule.h"

#include "util/Log.h"

#include <memory>
#include <string>

namespace fw {

std::auto_ptr<Rule> MakeRule_Or(Log& log, const std::string& name);

struct ComputeFunc_Or : public ComputeFunc {
  ComputeFunc_Or(Log& log)
    : ComputeFunc(log) {}
  virtual ~ComputeFunc_Or() {}
  virtual void operator() ();
  virtual std::auto_ptr<ComputeFunc> Clone() {
    return std::auto_ptr<ComputeFunc>(new ComputeFunc_Or(m_log));
  }
};

}

#endif // _Or_h_
