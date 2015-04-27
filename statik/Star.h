// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Star_h_
#define _Star_h_

#include "ComputeFunc.h"
#include "Rule.h"

#include <memory>
#include <string>

namespace statik {

std::auto_ptr<Rule> STAR(const std::string& name);

struct ComputeFunc_Star : public ComputeFunc {
  virtual ~ComputeFunc_Star() {}
  virtual void operator() (ConnectorAction::Action action, const IList& inode, const STree* initiator, int resize);
  virtual std::auto_ptr<ComputeFunc> Clone() {
    return std::auto_ptr<ComputeFunc>(new ComputeFunc_Star());
  }
};

std::auto_ptr<ComputeFunc> MakeComputeFunc_Star();

}

#endif // _Star_h_
