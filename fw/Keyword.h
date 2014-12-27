// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Keyword_h_
#define _Keyword_h_

/* Keyword rule
 *
 * Keywords recognize CharData input nodes, and match a specific string.
 */

#include "Connector.h"
#include "FWError.h"
#include "FWTree.h"
#include "Rule.h"
#include "State.h"

#include <memory>

namespace fw {

class KeywordRule : public Rule {
public:
  KeywordRule(Log& log, const std::string& str)
    : Rule(log, str, OS_SINGLE),
      m_str(str) {
    if (m_str.empty()) {
      throw FWError("Cannot create empty Keyword");
    }
  }
  virtual ~KeywordRule() {}

  const std::string& GetString() const { return m_str; }

  virtual void Update(Connector& connector, FWTree& x) const {
    m_log.info("Keyword: updating " + std::string(*this) + " at " + std::string(x));
    State& state = x.GetState();
    state.Clear();
    std::string matched;
    bool done = false;
    const IList* i = x.iconnection.istart;
    for (; i != NULL; i = i->right) {
      if (done) {
        state.GoComplete();
        break;
      }
      matched += i->value;
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

private:
  const std::string m_str;
};

}

#endif // _Keyword_h_
