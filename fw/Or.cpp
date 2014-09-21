// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Or.h"

#include "Connector.h"

#include <algorithm>
#include <string>
#include <vector>
using std::string;
using std::vector;

using namespace fw;

void OrRule::Reposition(Connector<ListDS>& connector, TreeDS& x, const ListDS& inode) const {
  //m_log.debug("Repositioning OrRule<ListDS> " + string(*this));
  x.Clear();
  x.istart = &inode;
  x.iend = &inode;
  if (x.children.size() == m_children.size()) {
    for (TreeDS::child_mod_iter i = x.children.begin();
         i != x.children.end(); ++i) {
      connector.RepositionNode(*i, inode);
    }
  } else {
    x.children.clear();
    for (child_iter i = m_children.begin(); i != m_children.end(); ++i) {
      std::auto_ptr<TreeDS> child(new TreeDS(i->MakeState(), &x));
      connector.RepositionNode(*child.get(), inode);
      x.children.push_back(child);
    }
  }
  (void) Update(connector, x, inode);
}

void OrRule::Reposition(Connector<TreeDS>& connector, TreeDS& x, const TreeDS& inode) const {
  // TODO (identical to Reposition<ListDS>?)
}

bool OrRule::Update(Connector<ListDS>& connector, TreeDS& x, const ListDS& inode) const {
  m_log.debug("Updating OrRule<ListDS> " + string(*this) + " at " + string(x) + " with inode " + string(inode));

  const DS* old_iend = x.iend;

  // Compute new state flags
  x.size = 0;
  OrState& state = x.GetState<OrState>();
  state.ok = true;
  state.bad = false;
  state.done = false;
  vector<const TreeDS*> oks;
  vector<const TreeDS*> bads;
  vector<const TreeDS*> dones;
  vector<const TreeDS*> completes;  // done and not ok
  for (TreeDS::child_iter i = x.children.begin(); i != x.children.end(); ++i) {
    State& istate = i->GetState();
    if (istate.ok || istate.done) {
      if (i->size > x.size) {
        x.size = i->size;
        x.iend = i->iend;
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
  return old_iend != x.iend;
}

bool OrRule::Update(Connector<TreeDS>& connector, TreeDS& x, const TreeDS& inode) const {
  // TODO
  return true;
}

std::auto_ptr<State> OrRule::MakeState() const {
  return std::auto_ptr<State>(new OrState(*this));
}
