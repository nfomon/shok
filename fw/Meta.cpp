// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Meta.h"

#include "FWTree.h"
#include "OutputFunc.h"
#include "RestartFunc.h"

#include "util/Log.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace fw;

auto_ptr<Rule> fw::MakeRule_Meta(const string& searchName) {
  return fw::MakeRule_Meta(searchName, searchName);
}

auto_ptr<Rule> fw::MakeRule_Meta(const string& name, const string& searchName) {
  return auto_ptr<Rule>(new Rule(name,
      MakeRestartFunc_None(),
      MakeComputeFunc_Meta(searchName),
      MakeOutputFunc_IValues(name)));
}

auto_ptr<ComputeFunc> fw::MakeComputeFunc_Meta(const string& searchName) {
  return auto_ptr<ComputeFunc>(new ComputeFunc_Meta(searchName));
}

void ComputeFunc_Meta::operator() () {
  g_log.info() << "Computing Meta at " << *m_node;
  State& state = m_node->GetState();
  state.GoBad();
  const IList& first = m_node->IStart();
  if (first.name == m_searchName) {
    m_node->GetConnector().Listen(*m_node, first);
    state.GoDone();
    const IList* second = first.right;
    if (second) {
      state.GoComplete();
      m_node->GetIConnection().SetEnd(*second);
    } else {
      m_node->GetIConnection().SetEnd(first);
    }
  }
  g_log.debug() << "Meta now at: " << *m_node;
}
