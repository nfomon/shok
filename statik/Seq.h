// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_Seq_h_
#define _statik_Seq_h_

#include "ComputeFunc.h"
#include "IList.h"
#include "ParseAction.h"
#include "Rule.h"
#include "STree.h"

#include <memory>
#include <string>

namespace statik {

std::auto_ptr<Rule> SEQ(const std::string& name);

struct ComputeFunc_Seq : public ComputeFunc {
  virtual ~ComputeFunc_Seq() {}
  virtual void operator() (ParseAction::Action action, const IList& inode, const STree* initiator);
  virtual std::auto_ptr<ComputeFunc> Clone() {
    return std::auto_ptr<ComputeFunc>(new ComputeFunc_Seq());
  }
};

std::auto_ptr<ComputeFunc> MakeComputeFunc_Seq();

}

#endif // _statik_Seq_h_
