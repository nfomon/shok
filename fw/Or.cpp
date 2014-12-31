// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Or.h"

#include "FWTree.h"

#include <memory>
#include <string>
#include <vector>
using std::auto_ptr;
using std::string;
using std::vector;

using namespace fw;

void OrRule::Update(FWTree& x) const {
  m_log.debug("Updating OrRule " + string(*this) + " at " + string(x));

  // Compute new state flags
  State& state = x.GetState();
  state.Clear();
  vector<const FWTree*> oks;
  vector<const FWTree*> bads;
  vector<const FWTree*> dones;
  vector<const FWTree*> completes;
  const FWTree* lockedChild = NULL;

  if (x.children.empty()) {
    throw FWError("Cannot update OrRule " + string(*this) + " that has no children");
  }
  x.GetIConnection().Restart(x.children.at(0).IStart());

  FWTree::child_mod_iter i = x.children.begin();
  bool haveSetEnd = false;
  for (; i != x.children.end(); ++i) {
    State& istate = i->GetState();
    bool thisChildLocked = false;
    if (istate.IsLocked()) {
      if (lockedChild) {
        throw FWError("OrRule " + string(*this) + " found more than one locked children");
      }
      m_log.info("OrRule " + string(*this) + " found locked child");
      m_log.info("Locked child state is " + string(istate));
      m_log.info("Locked child is " + string(*i));
      lockedChild = &*i;
      thisChildLocked = true;
      state.Lock();
    }
    if (!istate.IsBad() && !haveSetEnd) {
      if (&i->IStart() != &x.IStart()) {
        throw FWError("OrRule " + string(*this) + " and a child disagree about istart");
      }
      x.GetIConnection().SetEnd(i->IEnd());
      m_log.debug("OrRule " + string(*this) + " assigning iend " + string(x.IEnd()) + " from istate " + string(istate));
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
      throw FWError("OrRule " + string(*this) + " found istate " + string(istate) + " in unknown station");
    }
  }
  if (1 == completes.size()) {
    x.GetIConnection().SetEnd(completes.at(0)->IEnd());
    m_log.debug("OrRule " + string(*this) + " declares complete winner " + string(*completes.at(0)));
  } else if (1 == dones.size()) {
    x.GetIConnection().SetEnd(dones.at(0)->IEnd());
    m_log.debug("OrRule " + string(*this) + " declares done winner " + string(*dones.at(0)));
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

  m_log.debug("OrRule " + string(*this) + " now at " + string(x));
}
