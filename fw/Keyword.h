// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Keyword_h_
#define _Keyword_h_

/* Keyword rule
 *
 * Keywords recognize CharData input nodes, and match a specific string.
 */

#include "Char.h"
#include "Connector.h"
#include "FWError.h"
#include "FWTree.h"
#include "OData.h"
#include "Rule.h"

#include <memory>

namespace fw {

struct KeywordData : public OData {
  KeywordData(const std::string& str)
    : str(str) {}
  virtual ~KeywordData() {}
  std::string str;
  virtual operator std::string() const { return str; }
  virtual void Init() {}
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

  virtual void Reposition(Connector& connector, FWTree& x, const IList& inode) const {
    Update(connector, x);
  }

  virtual void Update(Connector& connector, FWTree& x) const {
    m_log.info("Keyword: updating " + std::string(*this) + " at " + std::string(x));
    KeywordState& state = x.GetState<KeywordState>();
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
      } else {
        // Just ok; keep going (if possible)
      }
      connector.Listen(x, *i);
    }
    x.iconnection.iend = i;
    x.iconnection.size = matched.size();
    if (state.IsEmitting()) {
      state.Lock();
    } else {
      state.Unlock();
    }
    m_log.debug("Keyword " + std::string(*this) + " now: " + std::string(x));
  }

  virtual std::auto_ptr<State> MakeState() const;
  virtual std::auto_ptr<OData> MakeData(const FWTree& x) const;
  virtual std::auto_ptr<OConnection> MakeOConnection(const FWTree& x) const;

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

std::auto_ptr<OData> KeywordRule::MakeData(const FWTree& x) const {
  return std::auto_ptr<OData>(new KeywordData(m_str));
}

std::auto_ptr<OConnection> KeywordRule::MakeOConnection(const FWTree& x) const {
  return std::auto_ptr<OConnection>(new OConnectionSingle(m_log, x));
}

}

#endif // _Keyword_h_
