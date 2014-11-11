// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Or.h"

#include "Connector.h"

#include <string>
#include <vector>
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
      std::auto_ptr<TreeDS> child(new TreeDS(i->MakeState(), &x));
      connector.RepositionNode(*child.get(), inode);
      x.children.push_back(child);
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
  state.ok = true;
  state.bad = false;
  state.done = false;
  vector<const TreeDS*> oks;
  vector<const TreeDS*> bads;
  vector<const TreeDS*> dones;
  vector<const TreeDS*> completes;  // done and not ok

  if (x.children.empty()) {
    throw FWError("Cannot update OrRule " + string(*this) + " that has no children");
  }

  TreeDS::child_mod_iter i = x.children.begin();
  for (; i != x.children.end(); ++i) {
    State& istate = i->GetState();
    if (istate.ok || istate.done) {
      // This disambiguation is perhaps silly.
      if (i->iconnection.size > x.iconnection.size) {
        if (i->iconnection.istart != x.iconnection.istart) {
          throw FWError("OrRule " + string(*this) + " and a child disagree about istart");
        }
        x.iconnection.iend = i->iconnection.iend;
        x.iconnection.size = i->iconnection.size;
      }
    }
    if (istate.ok) {
      oks.push_back(&*i);
    }
    if (istate.bad) {
      bads.push_back(&*i);
    }
    if (istate.done && !istate.ok) {
      completes.push_back(&*i);
    }
    if (istate.done) {
      dones.push_back(&*i);
    }

    // We want the hotlists of all children, no matter their state
    x.oconnection.hotlist.insert(i->oconnection.hotlist.begin(), i->oconnection.hotlist.end());
    i->oconnection.hotlist.clear();
  }
  if (1 == dones.size()) {
    x.oconnection.ostart = dones.at(0)->oconnection.ostart;
    x.oconnection.oend = dones.at(0)->oconnection.oend;
    x.oconnection.size = dones.at(0)->oconnection.size;
  }
  if (dones.size() > 1) {
    state.ok = true;
    state.bad = false;
    state.done = false;
  } else if (1 == dones.size() && 1 == completes.size()) {
    state.ok = false;
    state.bad = false;
    state.done = true;
  } else if (1 == dones.size()) {
    state.ok = true;
    state.bad = false;
    state.done = true;
  } else if (oks.empty() && dones.empty()) {
    state.ok = false;
    state.bad = true;
    state.done = false;
  }

  m_log.debug("OrRule " + string(*this) + " now at " + string(x));
  return old_iend != x.iconnection.iend || !x.oconnection.hotlist.empty();
}

std::auto_ptr<State> OrRule::MakeState() const {
  return std::auto_ptr<State>(new OrState(*this));
}
