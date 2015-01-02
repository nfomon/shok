// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Seq_h_
#define _Seq_h_

#include "ComputeFunc.h"
#include "Rule.h"

#include "util/Log.h"

#include <memory>
#include <string>

namespace fw {

std::auto_ptr<Rule> MakeRule_Seq(Log& log, const std::string& name);

struct ComputeFunc_Seq : public ComputeFunc {
  ComputeFunc_Seq(Log& log)
    : ComputeFunc(log) {}
  virtual ~ComputeFunc_Seq() {}
  virtual void operator() ();
  virtual std::auto_ptr<ComputeFunc> Clone() {
    return std::auto_ptr<ComputeFunc>(new ComputeFunc_Seq(m_log));
  }
};

std::auto_ptr<ComputeFunc> MakeComputeFunc_Seq(Log& log);

}

#endif // _Seq_h_
