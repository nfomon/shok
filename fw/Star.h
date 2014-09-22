// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Star_h_
#define _Star_h_

#include "Rule.h"

#include <memory>

namespace fw {

struct StarRule : public Rule {
  StarRule(Log& log, const std::string& name = "")
    : Rule(log, name) {}
  virtual ~StarRule() {}
  virtual void Reposition(Connector<ListDS>& connector, TreeDS& x, const ListDS& inode) const;
  virtual void Reposition(Connector<TreeDS>& connector, TreeDS& x, const TreeDS& inode) const;
  virtual bool Update(Connector<ListDS>& connector, TreeDS& x, const ListDS& inode) const;
  virtual bool Update(Connector<TreeDS>& connector, TreeDS& x, const TreeDS& inode) const;
  virtual std::auto_ptr<State> MakeState() const;
};

struct StarState : public RuleState {
  StarState(const StarRule& rule);
  virtual ~StarState() {}

  virtual void Clear();

  virtual operator std::string() const { return "[StarState:" + StateFlags() + "]"; }
};

}

#endif // _Star_h_
