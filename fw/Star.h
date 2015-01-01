// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Star_h_
#define _Star_h_

#include "ComputeFunc.h"
#include "Rule.h"

#include "util/Log.h"

#include <memory>
#include <string>

namespace fw {

std::auto_ptr<Rule> MakeRule_Star(Log& log, const std::string& name);

struct ComputeFunc_Star : public ComputeFunc {
  ComputeFunc_Star(Log& log)
    : ComputeFunc(log) {}
  virtual ~ComputeFunc_Star() {}
  virtual void operator() ();
  virtual std::auto_ptr<ComputeFunc> Clone() {
    return std::auto_ptr<ComputeFunc>(new ComputeFunc_Star(m_log));
  }
};

}

#endif // _Star_h_
