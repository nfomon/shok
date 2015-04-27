// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Seq_h_
#define _Seq_h_

#include "ComputeFunc.h"
#include "Rule.h"

#include <memory>
#include <string>

namespace statik {

std::auto_ptr<Rule> SEQ(const std::string& name);

struct ComputeFunc_Seq : public ComputeFunc {
  virtual ~ComputeFunc_Seq() {}
  virtual void operator() (ConnectorAction::Action action, const IList& inode, const STree* initiator, int resize);
  virtual std::auto_ptr<ComputeFunc> Clone() {
    return std::auto_ptr<ComputeFunc>(new ComputeFunc_Seq());
  }
};

std::auto_ptr<ComputeFunc> MakeComputeFunc_Seq();

}

#endif // _Seq_h_
