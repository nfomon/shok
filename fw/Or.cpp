// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Or.h"

#include "Connection.h"
#include "Connector.h"

#include <memory>
#include <string>
#include <vector>
using std::auto_ptr;
using std::string;
using std::vector;

using namespace fw;

void OrRule::Reposition(Connector& connector, FWTree& x, const IList& inode) const {
  m_log.debug("Repositioning OrRule " + string(*this) + " at " + string(x) + " with inode " + string(inode));
  RepositionAllChildrenOfNode(connector, x, inode);
  Update(connector, x);
}

void OrRule::Update(Connector& connector, FWTree& x) const {
  m_log.debug("Updating OrRule " + string(*this) + " at " + string(x));

  // Compute new state flags
  x.iconnection.iend = NULL;
  x.iconnection.size = 0;
  OrState& state = x.GetState<OrState>();
  state.Clear();
  vector<const FWTree*> oks;
  vector<const FWTree*> bads;
  vector<const FWTree*> dones;
  vector<const FWTree*> completes;
  const FWTree* lockedChild = NULL;

  if (x.children.empty()) {
    throw FWError("Cannot update OrRule " + string(*this) + " that has no children");
  }

  FWTree::child_mod_iter i = x.children.begin();
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
    if (!istate.IsBad()) {
      // This disambiguation is perhaps silly, and perhaps we needn't do this
      // at all here (since we'll assign iend later anyway).
      if (i->iconnection.size > x.iconnection.size) {
        if (i->iconnection.istart != x.iconnection.istart) {
          throw FWError("OrRule " + string(*this) + " and a child disagree about istart");
        }
        x.iconnection.iend = i->iconnection.iend;
        x.iconnection.size = i->iconnection.size;
        m_log.debug("OrRule " + string(*this) + " assigning iend " + (x.iconnection.iend ? string(*x.iconnection.iend) : "<null>") + " from istate " + string(istate));
      }
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
    x.iconnection.iend = completes.at(0)->iconnection.iend;
    x.iconnection.size = completes.at(0)->iconnection.size;
    m_log.debug("OrRule " + string(*this) + " declares complete winner " + string(*completes.at(0)));
    x.GetOConnection<OConnectionWinner>().DeclareWinner(*completes.at(0));
  } else if (1 == dones.size()) {
    x.iconnection.iend = dones.at(0)->iconnection.iend;
    x.iconnection.size = dones.at(0)->iconnection.size;
    m_log.debug("OrRule " + string(*this) + " declares done winner " + string(*dones.at(0)));
    x.GetOConnection<OConnectionWinner>().DeclareWinner(*dones.at(0));
  } else {
    // Nothing needs to happen; the OConnection will be ignored by the parent.
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
  m_log.debug(" - and it has istart " + (x.iconnection.istart ? string(*x.iconnection.istart) : "<null>") + " and iend " + (x.iconnection.iend ? string(*x.iconnection.iend): "<null>"));
}

auto_ptr<State> OrRule::MakeState() const {
  return auto_ptr<State>(new OrState(*this));
}

auto_ptr<OConnection> OrRule::MakeOConnection(const FWTree& x) const {
  return auto_ptr<OConnection>(new OConnectionWinner(m_log, x));
}
