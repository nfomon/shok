// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Root_h_
#define _Root_h_

/* Special Rule for the Root node */

#include "ComputeFunc.h"
#include "ConnectorAction.h"
#include "IList.h"
#include "Rule.h"
#include "STree.h"

#include <memory>
#include <string>

namespace statik {

class ComputeFunc_Root : public ComputeFunc {
public:
  ComputeFunc_Root(const std::string& name);
  virtual ~ComputeFunc_Root() {}
  virtual void operator() (ConnectorAction::Action action, const IList& inode, const STree* initiator);
  virtual std::auto_ptr<ComputeFunc> Clone() {
    return std::auto_ptr<ComputeFunc>(new ComputeFunc_Root(m_name));
  }

private:
  const std::string m_name;
};

std::auto_ptr<ComputeFunc> MakeComputeFunc_Root(const std::string& name);

}

#endif // _Root_h_
