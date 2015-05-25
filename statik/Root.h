// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_Root_h_
#define _statik_Root_h_

/* Special Rule for the Root node */

#include "ComputeFunc.h"
#include "List.h"
#include "ParseAction.h"
#include "Rule.h"
#include "STree.h"

#include <memory>
#include <string>

namespace statik {

class ComputeFunc_Root : public ComputeFunc {
public:
  ComputeFunc_Root(const std::string& name);
  virtual ~ComputeFunc_Root() {}
  virtual void operator() (ParseAction::Action action, const List& inode, const STree* initiator);
  virtual std::auto_ptr<ComputeFunc> Clone() {
    return std::auto_ptr<ComputeFunc>(new ComputeFunc_Root(m_name));
  }

private:
  const std::string m_name;
};

std::auto_ptr<ComputeFunc> MakeComputeFunc_Root(const std::string& name);

}

#endif // _statik_Root_h_
