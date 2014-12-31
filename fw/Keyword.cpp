// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Keyword.h"

#include "FWError.h"
#include "FWTree.h"

#include <string>
using std::string;

using namespace fw;

KeywordRule::KeywordRule(Log& log, const string& str)
  : Rule(log, str, RF_None, OS_SINGLE),
    m_str(str) {
  if (m_str.empty()) {
    throw FWError("Cannot create empty Keyword");
  }
}

void KeywordRule::Update(FWTree& x) const {
  m_log.info("Keyword: updating " + string(*this) + " at " + string(x));
  State& state = x.GetState();
  state.Clear();
  string matched;
  bool done = false;
  const IList* i = &x.IStart();
  for (; i != NULL; i = i->right) {
    x.GetIConnection().SetEnd(*i);
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
    x.GetConnector().Listen(x, *i);
  }
  if (state.IsEmitting()) {
    state.Lock();
  } else {
    state.Unlock();
  }
  m_log.debug("Keyword " + string(*this) + " now: " + string(x));
}
