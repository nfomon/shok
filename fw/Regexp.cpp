// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Regexp.h"

#include "Connector.h"
#include "FWTree.h"

#include <string>
using std::string;

using namespace fw;

RegexpRule::RegexpRule(Log& log, const std::string& name, const boost::regex& regex)
  : Rule(log, name, OS_VALUE),
    m_regex(regex) {
  if (m_regex.empty()) {
    throw FWError("Cannot create Regexp with empty regex");
  }
}

void RegexpRule::Update(Connector& connector, FWTree& x) const {
  m_log.info("Regexp: updating " + std::string(*this) + " at " + std::string(x));
  State& state = x.GetState();
  state.Clear();
  std::string str;
  const IList* i = &x.IStart();
  for (; i != NULL; i = i->right) {
    x.GetIConnection().SetEnd(*i);
    str += i->value;
    if (!boost::regex_match(str, m_regex)) {
      if (str.size() > 1) {
        if (boost::regex_match(str.begin(), str.end()-1, m_regex)) {
          state.GoComplete();
          break;
        }
      }
      state.GoBad();
      break;
    }
    connector.Listen(x, *i);
  }
  if (NULL == i) {
    boost::match_results<std::string::const_iterator> match_result;
    if (boost::regex_match(str, match_result, m_regex, boost::match_default | boost::match_partial)
        && match_result[0].matched) {
      state.GoDone();
    }
  }
  m_log.debug("Regexp " + std::string(*this) + " now: " + std::string(x));
}
