// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Star_h_
#define _Star_h_

#include "FWTree.h"
#include "IList.h"
#include "Rule.h"

#include <memory>

namespace fw {

struct StarRule : public Rule {
  StarRule(Log& log, const std::string& name = "")
    : Rule(log, name) {}
  virtual ~StarRule() {}
  virtual void Reposition(Connector& connector, FWTree& x, const IList& inode) const;
  virtual void Update(Connector& connector, FWTree& x) const;
  virtual std::auto_ptr<OConnection> MakeOConnection(const FWTree& x) const;
};

}

#endif // _Star_h_
