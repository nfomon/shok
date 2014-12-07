// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Keyword_h_
#define _Keyword_h_

/* Keyword and KeywordMeta rules
 *
 * Keywords recognize CharData input nodes, and match a specific string.
 *
 * KeywordMeta recognizes Keyword input nodes, and matches a specific keyword
 * node.
 */

#include "Connector.h"
#include "FWError.h"
#include "OData.h"
#include "Rule.h"

#include <boost/ptr_container/ptr_vector.hpp>

#include <memory>
#include <utility>
#include <vector>

namespace fw {

struct KeywordData : public OData {
  KeywordData(const std::string& str)
    : str(str) {}
  virtual ~KeywordData() {}
  std::string str;
  virtual operator std::string() const { return str; }
};

class KeywordRule;

struct KeywordState : public State {
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

  virtual void Reposition(Connector& connector, TreeDS& x, const IList& inode) const {
    x.Clear();
    x.iconnection.istart = &inode;
    x.iconnection.iend = NULL;
    x.oconnection.oleaf.reset(new IList(std::auto_ptr<OData>(new KeywordData(m_str))));
    x.oconnection.ostart = x.oconnection.oleaf.get();
    x.oconnection.oend = x.oconnection.oleaf.get();
    (void) Update(connector, x, NULL);
  }

  virtual bool Update(Connector& connector, TreeDS& x, const TreeDS* child) const {
    m_log.info("Keyword: updating " + std::string(*this) + " at " + std::string(x) + " with child " + (child ? std::string(*child) : "<null>"));
    const IList* old_iend = x.iconnection.iend;
    KeywordState& state = x.GetState<KeywordState>();
    bool was_emitting = state.IsEmitting();
    state.Clear();
    std::string matched;
    bool done = false;
    const IList* i = x.iconnection.istart;
    for (; i != NULL; i = i->right) {
      if (done) {
        state.GoComplete();
        break;
      }
      CharData idata = i->GetData<CharData>();
      matched += idata.c;
      if (m_str == matched) {
        state.GoDone();
        done = true;
      } else if (m_str.substr(0, matched.size()) != matched) {
        state.GoBad();
        break;
      }
      connector.Listen(x, *i);
    }
    x.iconnection.iend = i;
    x.iconnection.size = matched.size();
    if (state.IsEmitting() && !was_emitting) {
      x.oconnection.hotlist.insert(std::make_pair(x.oconnection.oleaf.get(), OP_INSERT));
    } else if (!state.IsEmitting() && was_emitting) {
      x.oconnection.hotlist.insert(std::make_pair(x.oconnection.oleaf.get(), OP_DELETE));
    }
    m_log.debug("Keyword " + std::string(*this) + " now: " + std::string(x));
    return old_iend != x.iconnection.iend || !x.oconnection.hotlist.empty();    // Should just be: has the state changed at all
  }

  virtual std::auto_ptr<State> MakeState() const;

private:
  const std::string m_str;
};

KeywordState::KeywordState(const KeywordRule& rule)
  : State(rule) {}

const KeywordRule& KeywordState::GetKeywordRule() const { return *dynamic_cast<const KeywordRule*>(&rule); }

KeywordState::operator std::string() const { return "kw " + rule.Name() + " (" + GetKeywordRule().GetString() + "):" + Print(); }

std::auto_ptr<State> KeywordRule::MakeState() const {
  return std::auto_ptr<State>(new KeywordState(*this));
}

class KeywordMetaRule;

struct KeywordMetaState : public State {
  KeywordMetaState(const KeywordMetaRule& rule);
  virtual ~KeywordMetaState() {}

  const KeywordMetaRule& GetKeywordMetaRule() const;

  virtual operator std::string() const;
};

class KeywordMetaRule : public Rule {
public:
  KeywordMetaRule(Log& log, const std::string& keyword, const std::string& name)
    : Rule(log, name),
      m_keyword(keyword) {}
  virtual ~KeywordMetaRule() {}

  const std::string& GetString() const { return m_keyword; }

  virtual void Reposition(Connector& connector, TreeDS& x, const IList& inode) const {
    x.Clear();
    x.iconnection.istart = &inode;
    x.iconnection.iend = NULL;
    x.oconnection.oleaf.reset(new IList(std::auto_ptr<OData>(new KeywordData(m_keyword))));
    x.oconnection.ostart = x.oconnection.oleaf.get();
    x.oconnection.oend = x.oconnection.oleaf.get();
    (void) Update(connector, x, NULL);
  }

  virtual bool Update(Connector& connector, TreeDS& x, const TreeDS* child) const {
    m_log.info("KeywordMeta: updating " + std::string(*this) + " at " + std::string(x) + " with child " + (child ? std::string(*child) : "<null>"));
    const IList* old_iend = x.iconnection.iend;
    State& state = x.GetState();
    state.GoBad();
    bool was_emitting = state.IsEmitting();
    const IList* first = x.iconnection.istart;
    const KeywordData* firstData = dynamic_cast<KeywordData*>(&first->GetData());
    if (firstData && firstData->str == m_keyword) {
      connector.Listen(x, *first);
      state.GoDone();
      x.iconnection.size = 1;
      const IList* second = first->right;
      if (second) {
        state.GoComplete();
        x.iconnection.iend = second;
      } else {
        x.iconnection.iend = NULL;
      }
    }
    if (state.IsEmitting() && !was_emitting) {
      x.oconnection.hotlist.insert(std::make_pair(x.oconnection.oleaf.get(), OP_INSERT));
    } else if (!state.IsEmitting() && was_emitting) {
      x.oconnection.hotlist.insert(std::make_pair(x.oconnection.oleaf.get(), OP_DELETE));
    }
    m_log.debug("KeywordMeta " + std::string(*this) + " now: " + std::string(x));
    return old_iend != x.iconnection.iend || !x.oconnection.hotlist.empty();    // Should just be: has the state changed at all
  }

  virtual std::auto_ptr<State> MakeState() const;

private:
  std::string m_keyword;
};

KeywordMetaState::KeywordMetaState(const KeywordMetaRule& rule)
  : State(rule) {}

const KeywordMetaRule& KeywordMetaState::GetKeywordMetaRule() const { return *dynamic_cast<const KeywordMetaRule*>(&rule); }

KeywordMetaState::operator std::string() const { return "kwmeta " + rule.Name() + " (" + GetKeywordMetaRule().GetString() + "):" + Print(); }

std::auto_ptr<State> KeywordMetaRule::MakeState() const {
  return std::auto_ptr<State>(new KeywordMetaState(*this));
}

}

#endif // _Keyword_h_
