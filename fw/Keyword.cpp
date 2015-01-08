// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Keyword.h"

#include "FWError.h"
#include "FWTree.h"
#include "OutputFunc.h"
#include "RestartFunc.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace fw;

auto_ptr<Rule> fw::MakeRule_Keyword(Log& log, const string& str) {
  return fw::MakeRule_Keyword(log, str, str);
}

auto_ptr<Rule> fw::MakeRule_Keyword(Log& log, const string& name, const string& str) {
  return auto_ptr<Rule>(new Rule(log, name,
      MakeRestartFunc_None(log),
      MakeComputeFunc_Keyword(log, str),
      MakeOutputFunc_Basic(log, name)));
}

auto_ptr<ComputeFunc> fw::MakeComputeFunc_Keyword(Log& log, const string& str) {
  return auto_ptr<ComputeFunc>(new ComputeFunc_Keyword(log, str));
}

ComputeFunc_Keyword::ComputeFunc_Keyword(Log& log, const string& str)
  : ComputeFunc(log),
    m_str(str) {
  if (m_str.empty()) {
    throw FWError("Cannot create empty Keyword");
  }
}

void ComputeFunc_Keyword::operator() () {
  m_log.info("Computing Keyword at " + string(*m_node));
  State& state = m_node->GetState();
  state.Clear();
  string matched;
  bool done = false;
  const IList* i = &m_node->IStart();
  for (; i != NULL; i = i->right) {
    m_node->GetIConnection().SetEnd(*i);
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
    m_node->GetConnector().Listen(*m_node, *i);
  }
  if (state.IsEmitting()) {
    state.Lock();
  } else {
    state.Unlock();
  }
  m_log.debug("Keyword now at: " + string(*m_node));
}
