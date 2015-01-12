// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Or.h"

#include "FWLog.h"
#include "FWTree.h"
#include "OutputFunc.h"
#include "RestartFunc.h"

#include <memory>
#include <string>
#include <vector>
using std::auto_ptr;
using std::string;
using std::vector;

using namespace fw;

auto_ptr<Rule> fw::MakeRule_Or(const string& name) {
  return auto_ptr<Rule>(new Rule(name,
      MakeRestartFunc_AllChildrenOfNode(),
      MakeComputeFunc_Or(),
      MakeOutputFunc_Winner()));
}

auto_ptr<ComputeFunc> fw::MakeComputeFunc_Or() {
  return auto_ptr<ComputeFunc>(new ComputeFunc_Or());
}

void ComputeFunc_Or::operator() () {
  g_log.debug() << "Computing Or at " << *m_node;

  // Compute new state flags
  State& state = m_node->GetState();
  state.Clear();
  vector<const FWTree*> oks;
  vector<const FWTree*> bads;
  vector<const FWTree*> dones;
  vector<const FWTree*> completes;
  const FWTree* lockedChild = NULL;

  if (m_node->children.empty()) {
    throw FWError("Cannot compute Or at " + string(*m_node) + " that has no children");
  }
  m_node->GetIConnection().Restart(m_node->children.at(0).IStart());

  FWTree::child_mod_iter i = m_node->children.begin();
  bool haveSetEnd = false;
  for (; i != m_node->children.end(); ++i) {
    State& istate = i->GetState();
    bool thisChildLocked = false;
    if (istate.IsLocked()) {
      if (lockedChild) {
        throw FWError("Computing Or at " + string(*m_node) + " found more than one locked children");
      }
      g_log.info() << "Computing Or at " << *m_node << " found locked child";
      g_log.info() << "Locked child state is " << istate;
      g_log.info() << "Locked child is " << *i;
      lockedChild = &*i;
      thisChildLocked = true;
      state.Lock();
    }
    if (!istate.IsBad() && !haveSetEnd) {
      if (&i->IStart() != &m_node->IStart()) {
        throw FWError("Computing Or at " + string(*m_node) + " and a child disagree about istart");
      }
      m_node->GetIConnection().SetEnd(i->IEnd());
      g_log.debug() << "Computing Or at " << *m_node << " assigning iend " << m_node->IEnd() << " from istate " << istate;
      haveSetEnd = true;
    }
    if (istate.IsOK()) {
      if (thisChildLocked) {
        bads.clear();
        oks.clear();
        dones.clear();
        completes.clear();
        oks.push_back(lockedChild);
      } else if (!lockedChild) {
        oks.push_back(&*i);
      }
    } else if (istate.IsBad()) {
      if (thisChildLocked) {
        bads.clear();
        oks.clear();
        dones.clear();
        completes.clear();
        bads.push_back(lockedChild);
      } else if (!lockedChild) {
        bads.push_back(&*i);
      }
    } else if (istate.IsDone()) {
      if (thisChildLocked) {
        bads.clear();
        oks.clear();
        dones.clear();
        completes.clear();
        dones.push_back(lockedChild);
      } else if (!lockedChild) {
        dones.push_back(&*i);
      }
    } else if (istate.IsComplete()) {
      if (thisChildLocked) {
        bads.clear();
        oks.clear();
        dones.clear();
        completes.clear();
        completes.push_back(lockedChild);
      } else if (!lockedChild) {
        completes.push_back(&*i);
      }
    } else {
      throw FWError("Computing Or at " + string(*m_node) + " found istate " + string(istate) + " in unknown station");
    }
  }
  if (1 == completes.size()) {
    m_node->GetIConnection().SetEnd(completes.at(0)->IEnd());
    g_log.debug() << "Computing Or at " << *m_node << " declares complete winner " << *completes.at(0);
  } else if (1 == dones.size()) {
    m_node->GetIConnection().SetEnd(dones.at(0)->IEnd());
    g_log.debug() << "Computing Or at " << *m_node << " declares done winner " << *dones.at(0);
  }
  if (1 == completes.size()) {
    state.GoComplete();
  } else if (1 == dones.size()) {
    state.GoDone();
  } else if (dones.size() + oks.size() >= 1) {
    state.GoOK();
  } else {
    state.GoBad();
  }

  g_log.debug() << "Or now at: " << *m_node;
}
