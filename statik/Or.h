// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_Or_h_
#define _statik_Or_h_

#include "ComputeFunc.h"
#include "ConnectorAction.h"
#include "IList.h"
#include "Rule.h"
#include "STree.h"

#include <memory>
#include <string>

namespace statik {

std::auto_ptr<Rule> OR(const std::string& name);

struct ComputeFunc_Or : public ComputeFunc {
  virtual ~ComputeFunc_Or() {}
  virtual void operator() (ConnectorAction::Action action, const IList& inode, const STree* initiator);
  virtual std::auto_ptr<ComputeFunc> Clone() {
    return std::auto_ptr<ComputeFunc>(new ComputeFunc_Or());
  }
};

std::auto_ptr<ComputeFunc> MakeComputeFunc_Or();

}

#endif // _statik_Or_h_
