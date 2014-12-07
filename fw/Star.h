// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Star_h_
#define _Star_h_

#include "Rule.h"
#include "State.h"

#include <memory>

namespace fw {

struct StarRule : public Rule {
  StarRule(Log& log, const std::string& name = "")
    : Rule(log, name) {}
  virtual ~StarRule() {}
  virtual void Reposition(Connector& connector, TreeDS& x, const IList& inode) const;
  virtual bool Update(Connector& connector, TreeDS& x, const TreeDS* child) const;
  virtual std::auto_ptr<State> MakeState() const;
};

struct StarState : public State {
  StarState(const StarRule& rule)
    : State(rule, /*startDone*/ true) {}
  virtual ~StarState() {}

  virtual operator std::string() const { return "Star " + rule.Name() + ":" + Print(); }
};

}

#endif // _Star_h_
