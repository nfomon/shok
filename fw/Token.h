// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Token_h_
#define _Token_h_

#include "Connector.h"
#include "FWError.h"
#include "Rule.h"

#include <boost/ptr_container/ptr_vector.hpp>

#include <memory>
#include <utility>
#include <vector>

namespace fw {

class TokenRule;

struct TokenState : public RuleState {
  TokenState(const TokenRule& rule);
  virtual ~TokenState() {}

  const TokenRule& GetTokenRule() const;

  virtual operator std::string() const;
};

class TokenRule : public Rule {
public:
  TokenRule(Log& log, const std::string& str)
    : Rule(log, str),
      m_str(str) {
    if (m_str.empty()) {
      throw FWError("Cannot create empty Token");
    }
  }
  virtual ~TokenRule() {}

  const std::string& GetString() const { return m_str; }

  virtual void Reposition(Connector<ListDS>& connector, TreeDS& x, const ListDS& inode) const {
    throw FWError("TokenRule::Reposition<TreeDS> is unimplemented");
  }

  virtual void Reposition(Connector<TreeDS>& connector, TreeDS& x, const TreeDS& inode) const {
    x.Clear();
    x.istart = &inode;
    x.iend = &inode;
    (void) Update(connector, x, inode);
  }

  virtual bool Update(Connector<ListDS>& connector, TreeDS& x, const ListDS& inode) const {
    throw FWError("Inappropriate TreeDS Update of TokenRule");
  }

  virtual bool Update(Connector<TreeDS>& connector, TreeDS& x, const TreeDS& inode) const {
    m_log.info("Token: updating " + std::string(*this) + " at " + std::string(x) + " with inode " + std::string(inode));

    TokenState& state = x.GetState<TokenState>();
    KeywordState& istate = inode.GetState<KeywordState>();
    const KeywordRule& irule = istate.GetKeywordRule();
    const std::string& istr = irule.GetString();
    if (m_str == istr) {
      state.ok = false;
      state.bad = false;
      state.done = true;
    } else {
      state.ok = false;
      state.bad = true;
      state.done = false;
    }

    return true;
  }

  virtual std::auto_ptr<State> MakeState() const;

private:
  const std::string m_str;
};

TokenState::TokenState(const TokenRule& rule)
  : RuleState(rule) {}

const TokenRule& TokenState::GetTokenRule() const { return *dynamic_cast<const TokenRule*>(&rule); }

TokenState::operator std::string() const { return "[TokenState \"" + GetTokenRule().GetString() + "\":" + StateFlags() + "]"; }

std::auto_ptr<State> TokenRule::MakeState() const {
  return std::auto_ptr<State>(new TokenState(*this));
}

}

#endif // _Token_h_
