// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Or_h_
#define _Or_h_

#include "Rule.h"

#include <memory>
#include <utility>
#include <vector>

namespace fw {

struct OrState;

struct OrRule : public Rule {
  OrRule(Log& log, const std::string& name = "")
    : Rule(log, name) {}
  virtual ~OrRule() {}
  virtual void Reposition(Connector<ListDS>& connector, TreeDS& x, const ListDS& inode) const;
  virtual void Reposition(Connector<TreeDS>& connector, TreeDS& x, const TreeDS& inode) const;
  virtual bool Update(Connector<ListDS>& connector, TreeDS& x, const ListDS& inode) const;
  virtual bool Update(Connector<TreeDS>& connector, TreeDS& x, const TreeDS& inode) const;
  virtual std::auto_ptr<State> MakeState() const;
};

struct OrState : public RuleState {
public:
  OrState(const OrRule& rule)
    : RuleState(rule) {}
  virtual ~OrState() {}

  virtual operator std::string() const { return "[OrState:" + StateFlags() + "]"; }
};

}

#endif // _Or_h_
