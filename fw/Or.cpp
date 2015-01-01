// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Or.h"

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

auto_ptr<Rule> fw::MakeRule_Or(Log& log, const string& name) {
  return auto_ptr<Rule>(new Rule(log, name,
      auto_ptr<RestartFunc>(new RestartFunc_AllChildrenOfNode(log)),
      auto_ptr<ComputeFunc>(new ComputeFunc_Or(log)),
      auto_ptr<OutputFunc>(new OutputFunc_Winner(log))));
}

void ComputeFunc_Or::operator() () {
  m_log.debug("Computing Or at " + string(*m_node));

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
      m_log.info("Computing Or at " + string(*m_node) + " found locked child");
      m_log.info("Locked child state is " + string(istate));
      m_log.info("Locked child is " + string(*i));
      lockedChild = &*i;
      thisChildLocked = true;
      state.Lock();
    }
    if (!istate.IsBad() && !haveSetEnd) {
      if (&i->IStart() != &m_node->IStart()) {
        throw FWError("Computing Or at " + string(*m_node) + " and a child disagree about istart");
      }
      m_node->GetIConnection().SetEnd(i->IEnd());
      m_log.debug("Computing Or at " + string(*m_node) + " assigning iend " + string(m_node->IEnd()) + " from istate " + string(istate));
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
    m_log.debug("Computing Or at " + string(*m_node) + " declares complete winner " + string(*completes.at(0)));
  } else if (1 == dones.size()) {
    m_node->GetIConnection().SetEnd(dones.at(0)->IEnd());
    m_log.debug("Computing Or at " + string(*m_node) + " declares done winner " + string(*dones.at(0)));
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

  m_log.debug("Or now at: " + string(*m_node));
}
