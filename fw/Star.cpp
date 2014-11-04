// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Star.h"

#include "Connector.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <string>
#include <vector>
using std::string;
using std::vector;

using namespace fw;

void StarRule::Reposition(Connector<ListDS>& connector, TreeDS& x, const ListDS& inode) const {
  m_log.debug("Repositioning StarRule<ListDS> " + string(*this) + " at " + string(x) + " with " + string(inode));
  if (x.children.empty()) {
    if (m_children.size() != 1) {
      throw FWError("StarRule must have exactly one child; found " + lexical_cast<string>(m_children.size()));
    }
    std::auto_ptr<TreeDS> child(new TreeDS(m_children.at(0).MakeState(), &x));
    x.children.push_back(child);
  } else if (x.children.size() > 1) {
    // Remove all children > 1
    for (TreeDS::child_mod_iter i = x.children.begin() + 1;
         i != x.children.end(); ++i) {
      connector.ClearNode(*i);
    }
    x.children.erase(x.children.begin() + 1, x.children.end());
  }
  x.istart = &inode;
  connector.RepositionNode(x.children.at(0), inode);
  (void) Update(connector, x, NULL);
}

void StarRule::Reposition(Connector<TreeDS>& connector, TreeDS& x, const TreeDS& inode) const {
  // TODO (identical to Reposition<ListDS>?)
  throw FWError("StarRule<TreeDS>::Reposition unimplemented");
}

bool StarRule::Update(Connector<ListDS>& connector, TreeDS& x, const TreeDS* updated_child) const {
  m_log.debug("Updating StarRule<ListDS> " + string(*this) + " at " + string(x) + " with child " + (updated_child ? string(*updated_child) : "<null>"));

  const DS* old_iend = x.iend;
  x.iend = x.istart;

  // Compute new state flags
  x.size = 0;
  StarState& state = x.GetState<StarState>();
  state.ok = true;
  state.bad = false;
  state.done = true;

  if (x.children.empty()) {
    throw FWError("StarRule<ListDS>::Update: StarState " + string(x) + " must have children");
  }

  // Iterate over children, either existing or being created, starting at the
  // updated_child, so long as our last child is complete.
  bool finished = false;
  TreeDS::child_mod_iter child = x.children.begin();
  if (updated_child) {
    for (child = x.children.begin(); child != x.children.end(); ++child) {
      if (updated_child == &*child) { break; }
    }
  }
  while (!finished) {
    if (child != x.children.end()) {
      // Existing child
      if (x.iend != child->istart) {
        throw FWError("StarRule " + string(*this) + " found iend->istart mismatch; we could correct this, but let's not...");
        //connector.RepositionNode(*child, *dynamic_cast<const ListDS*>(x.iend));   // Could correct it like this
      }
    } else {
      // New child
      std::auto_ptr<TreeDS> newChild(new TreeDS(m_children.at(0).MakeState(), &x));
      x.children.push_back(newChild);
      child = x.children.end() - 1;
      connector.RepositionNode(*child, *dynamic_cast<const ListDS*>(x.iend));
    }
    x.iend = child->iend;

    // The child has updated, and no matter what state it's in, we should take
    // its hotlist.
    x.hotlist.insert(child->hotlist.begin(), child->hotlist.end());
    child->hotlist.clear();

    // Now check the child's state, and see if we can keep going.
    State& istate = child->GetState();
    if (istate.bad) {
      m_log.debug("StarRule " + string(*this) + " has gone bad");
      state.ok = false;
      state.bad = true;
      state.done = false;
      // Clear any subsequent children
      for (TreeDS::child_mod_iter i = child; i != x.children.end(); ++i) {
        connector.ClearNode(*i);
      }
      x.children.erase(child, x.children.end());
      finished = true;
    } else if (istate.done && !istate.ok) {
      // Cool, keep going!
    } else if (istate.ok) {
      if (child->iend != NULL) {
        throw FWError("StarRule found incomplete inode that is only ok; not allowed");
      }
      state.ok = true;
      state.bad = false;
      state.done = false;
      finished = true;
    } else {
      throw FWError("StarRule " + string(*this) + " child is in unexpected state");
    }

    ++child;
  }

  m_log.debug("StarRule " + string(*this) + " done update; now has state " + string(x) + " and hotlist size is " + boost::lexical_cast<string>(x.hotlist.size()));
  return old_iend != x.iend || !x.hotlist.empty();
}

bool StarRule::Update(Connector<TreeDS>& connector, TreeDS& x, const TreeDS* child) const {
  // TODO
  throw FWError("StarRule<TreeDS>::Update() is unimplemented");
}

std::auto_ptr<State> StarRule::MakeState() const {
  return std::auto_ptr<State>(new StarState(*this));
}

StarState::StarState(const StarRule& rule)
  : RuleState(rule) {
  done = true;
}

void StarState::Clear() {
  ok = true;
  bad = false;
  done = true;
  locked = false;
}
