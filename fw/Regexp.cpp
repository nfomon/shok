// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Regexp.h"

#include "FWTree.h"
#include "OutputFunc.h"
#include "RestartFunc.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace fw;

auto_ptr<Rule> fw::MakeRule_Regexp(Log& log, const string& name, const boost::regex& regex) {
  return auto_ptr<Rule>(new Rule(log, name,
      auto_ptr<RestartFunc>(new RestartFunc_None(log)),
      auto_ptr<ComputeFunc>(new ComputeFunc_Regexp(log, regex)),
      auto_ptr<OutputFunc>(new OutputFunc_Value(log, name))));
}

ComputeFunc_Regexp::ComputeFunc_Regexp(Log& log, const boost::regex& regex)
  : ComputeFunc(log),
    m_regex(regex) {
  if (m_regex.empty()) {
    throw FWError("Cannot create Regexp with empty regex");
  }
}

void ComputeFunc_Regexp::operator() () {
  m_log.info("Computing Regexp at " + string(*m_node));
  State& state = m_node->GetState();
  state.Clear();
  std::string str;
  const IList* i = &m_node->IStart();
  for (; i != NULL; i = i->right) {
    m_node->GetIConnection().SetEnd(*i);
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
    m_node->GetConnector().Listen(*m_node, *i);
  }
  if (NULL == i) {
    boost::match_results<std::string::const_iterator> match_result;
    if (boost::regex_match(str, match_result, m_regex, boost::match_default | boost::match_partial)
        && match_result[0].matched) {
      state.GoDone();
    }
  }
  m_log.debug("Regexp now at " + string(*m_node));
}
