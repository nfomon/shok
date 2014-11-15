// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Seq_h_
#define _Seq_h_

#include "Rule.h"

#include <memory>

namespace fw {

struct SeqRule : public Rule {
  SeqRule(Log& log, const std::string& name = "")
    : Rule(log, name) {}
  virtual ~SeqRule() {}
  virtual void Reposition(Connector& connector, TreeDS& x, const IList& inode) const;
  virtual bool Update(Connector& connector, TreeDS& x, const TreeDS* child) const;
  virtual std::auto_ptr<State> MakeState() const;
};

struct SeqState : public RuleState {
  SeqState(const SeqRule& rule)
    : RuleState(rule) {}
  virtual ~SeqState() {}

  virtual operator std::string() const { return "[SeqState:" + StateFlags() + "]"; }
};

}

#endif // _Seq_h_
