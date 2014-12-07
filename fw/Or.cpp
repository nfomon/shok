// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Or.h"

#include "Connector.h"

#include <memory>
#include <string>
#include <vector>
using std::auto_ptr;
using std::string;
using std::vector;

using namespace fw;

void OrRule::Reposition(Connector& connector, TreeDS& x, const IList& inode) const {
  m_log.debug("Repositioning OrRule " + string(*this) + " at " + string(x) + " with inode " + string(inode));
  x.Clear();
  x.iconnection.istart = &inode;
  x.iconnection.iend = NULL;
  if (x.children.empty()) {
    for (child_iter i = m_children.begin(); i != m_children.end(); ++i) {
      x.children.push_back(auto_ptr<TreeDS>(new TreeDS(i->MakeState(), &x)));
      connector.RepositionNode(x.children.back(), inode);
    }
  } else if (x.children.size() == m_children.size()) {
    for (TreeDS::child_mod_iter i = x.children.begin();
         i != x.children.end(); ++i) {
      connector.RepositionNode(*i, inode);
    }
  } else {
    throw FWError("Inappropriate # children in Or node " + string(x));
  }
  (void) Update(connector, x, NULL);
}

bool OrRule::Update(Connector& connector, TreeDS& x, const TreeDS* child) const {
  m_log.debug("Updating OrRule " + string(*this) + " at " + string(x) + " with child " + (child ? string(*child) : "<null>"));

  const IList* old_iend = x.iconnection.iend;

  // Compute new state flags
  x.iconnection.iend = NULL;
  x.iconnection.size = 0;
  x.oconnection.Clear();
  OrState& state = x.GetState<OrState>();
  state.Clear();
  vector<const TreeDS*> oks;
  vector<const TreeDS*> bads;
  vector<const TreeDS*> dones;
  vector<const TreeDS*> completes;

  if (x.children.empty()) {
    throw FWError("Cannot update OrRule " + string(*this) + " that has no children");
  }

  TreeDS::child_mod_iter i = x.children.begin();
  for (; i != x.children.end(); ++i) {
    State& istate = i->GetState();
    if (!istate.IsBad()) {
      // This disambiguation is perhaps silly.
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
      oks.push_back(&*i);
    } else if (istate.IsBad()) {
      bads.push_back(&*i);
    } else if (istate.IsDone()) {
      dones.push_back(&*i);
    } else if (istate.IsComplete()) {
      completes.push_back(&*i);
    } else {
      throw FWError("OrRule " + string(*this) + " found istate " + string(istate) + " in unknown station");
    }

    // We want the hotlists of all children, no matter their state
    x.oconnection.hotlist.insert(i->oconnection.hotlist.begin(), i->oconnection.hotlist.end());
    i->oconnection.hotlist.clear();
  }
  if (x.oconnection.hotlist.size() > 0) {
    m_log.debug("OrRule " + string(*this) + " now has " + boost::lexical_cast<string>(x.oconnection.hotlist.size()) + " hotlist items");
  }
  if (1 == completes.size()) {
    x.oconnection.ostart = completes.at(0)->oconnection.ostart;
    x.oconnection.oend = completes.at(0)->oconnection.oend;
  } else if (1 == dones.size()) {
    x.oconnection.ostart = dones.at(0)->oconnection.ostart;
    x.oconnection.oend = dones.at(0)->oconnection.oend;
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
  return old_iend != x.iconnection.iend || !x.oconnection.hotlist.empty();
}

auto_ptr<State> OrRule::MakeState() const {
  return auto_ptr<State>(new OrState(*this));
}
