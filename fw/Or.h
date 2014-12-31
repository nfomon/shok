// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Or_h_
#define _Or_h_

#include "Rule.h"

namespace fw {

struct OrRule : public Rule {
  OrRule(Log& log, const std::string& name = "")
    : Rule(log, name, RF_AllChildrenOfNode, OS_WINNER) {}
  virtual ~OrRule() {}
  virtual void Update(FWTree& x) const;
};

}

#endif // _Or_h_
