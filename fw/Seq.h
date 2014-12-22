// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Seq_h_
#define _Seq_h_

#include "FWTree.h"
#include "IList.h"
#include "Rule.h"
#include "State.h"

#include <memory>

namespace fw {

struct SeqRule : public Rule {
  SeqRule(Log& log, const std::string& name = "")
    : Rule(log, name) {}
  virtual ~SeqRule() {}
  virtual void Reposition(Connector& connector, FWTree& x, const IList& inode) const;
  virtual void Update(Connector& connector, FWTree& x) const;
  virtual std::auto_ptr<State> MakeState() const;
  virtual std::auto_ptr<OConnection> MakeOConnection(const FWTree& x) const;
};

struct SeqState : public State {
  SeqState(const SeqRule& rule)
    : State(rule) {}
  virtual ~SeqState() {}

  virtual operator std::string() const { return "Seq " + rule.Name() + ":" + Print(); }
};

}

#endif // _Seq_h_