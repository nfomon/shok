// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Keyword_h_
#define _Keyword_h_

#include "Connector.h"
#include "FWError.h"
#include "Rule.h"

#include <boost/ptr_container/ptr_vector.hpp>

#include <memory>
#include <utility>
#include <vector>

namespace fw {

class KeywordRule;

struct KeywordState : public RuleState {
  KeywordState(const KeywordRule& rule);
  virtual ~KeywordState() {}

  const KeywordRule& GetKeywordRule() const;

  virtual operator std::string() const;
};

class KeywordRule : public Rule {
public:
  KeywordRule(Log& log, const std::string& str)
    : Rule(log, str),
      m_str(str) {
    if (m_str.empty()) {
      throw FWError("Cannot create empty Keyword");
    }
  }
  virtual ~KeywordRule() {}

  const std::string& GetString() const { return m_str; }

  virtual void Reposition(Connector<ListDS>& connector, TreeDS& x, const ListDS& inode) const {
    x.Clear();
    x.istart = &inode;
    x.iend = &inode;
    (void) Update(connector, x, NULL);
  }

  virtual void Reposition(Connector<TreeDS>& connector, TreeDS& x, const TreeDS& inode) const {
    throw FWError("KeywordRule::Reposition<TreeDS> is unimplemented");
  }

  virtual bool Update(Connector<ListDS>& connector, TreeDS& x, const TreeDS* child) const {
    m_log.info("Keyword: updating " + std::string(*this) + " at " + std::string(x) + " with child " + (child ? std::string(*child) : "<null>"));
    const DS* old_iend = x.iend;
    const ListDS* istart = dynamic_cast<const ListDS*>(x.istart);
    KeywordState& state = x.GetState<KeywordState>();
    std::string matched;
    state.ok = true;
    state.bad = false;
    state.done = false;
    bool done = false;
    const ListDS* i = istart;
    for (; i != NULL; i = i->right) {
      if (done) {
        state.ok = false;
        state.bad = false;
        state.done = true;
        //m_log.debug (" - - done break");
        break;
      }
      CharState istate = i->GetState<CharState>();
      matched += istate.c;
      if (m_str == matched) {
        state.ok = true;
        state.bad = false;
        state.done = true;
        done = true;
        //m_log.debug (" - - matched continue");
      } else if (m_str.substr(0, matched.size()) != matched) {
        state.ok = false;
        state.bad = true;
        state.done = false;
        //m_log.debug (" - - bad break: '" + m_str.substr(0, matched.size()) + "' - '" + matched + "'");
        break;
      }
      connector.Listen(x, *i);
    }
    x.iend = i;
    x.size = matched.size();
    m_log.debug("Keyword " + std::string(*this) + " now: " + std::string(x));
    return old_iend != x.iend;
  }

  virtual bool Update(Connector<TreeDS>& connector, TreeDS& x, const TreeDS* child) const {
    throw FWError("Inappropriate TreeDS Update of KeywordRule");
  }

  virtual std::auto_ptr<State> MakeState() const;

private:
  const std::string m_str;
};

KeywordState::KeywordState(const KeywordRule& rule)
  : RuleState(rule) {}

const KeywordRule& KeywordState::GetKeywordRule() const { return *dynamic_cast<const KeywordRule*>(&rule); }

KeywordState::operator std::string() const { return "[KeywordState \"" + GetKeywordRule().GetString() + "\":" + StateFlags() + "]"; }

std::auto_ptr<State> KeywordRule::MakeState() const {
  return std::auto_ptr<State>(new KeywordState(*this));
}

}

#endif // _Keyword_h_
