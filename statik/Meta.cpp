// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Meta.h"

#include "IncParser.h"
#include "OutputFunc.h"
#include "SLog.h"
#include "STree.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace statik;

auto_ptr<Rule> statik::META(const string& searchName) {
  return statik::META(searchName, searchName);
}

auto_ptr<Rule> statik::META(const string& name, const string& searchName) {
  return auto_ptr<Rule>(new Rule(name,
      MakeComputeFunc_Meta(searchName),
      MakeOutputFunc_IValues(name)));
}

auto_ptr<ComputeFunc> statik::MakeComputeFunc_Meta(const string& searchName) {
  return auto_ptr<ComputeFunc>(new ComputeFunc_Meta(searchName));
}

void ComputeFunc_Meta::operator() (ParseAction::Action action, const List& inode, const STree* initiator) {
  g_log.info() << "Computing Meta at " << *m_node << " with inode "<< inode;
  State& state = m_node->GetState();
  state.Clear();
  const List& first = m_node->IStart();
  m_node->GetIncParser().Listen(*m_node, first);
  if (first.name == m_searchName) {
    state.GoDone();
    const List* second = first.right;
    if (second) {
      state.GoComplete();
      m_node->GetIConnection().SetEnd(*second);
    } else {
      m_node->GetIConnection().SetEnd(first);
    }
  } else {
    state.GoBad();
  }
  g_log.debug() << "Meta now at: " << *m_node;
}
