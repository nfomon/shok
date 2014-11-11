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

void StarRule::Reposition(Connector& connector, TreeDS& x, const IList& inode) const {
  m_log.debug("Repositioning StarRule " + string(*this) + " at " + string(x) + " with " + string(inode));
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
  x.iconnection.Clear();
  x.iconnection.istart = &inode;
  connector.RepositionNode(x.children.at(0), inode);
  (void) Update(connector, x, NULL);
}

bool StarRule::Update(Connector& connector, TreeDS& x, const TreeDS* updated_child) const {
  m_log.debug("Updating StarRule " + string(*this) + " at " + string(x) + " with child " + (updated_child ? string(*updated_child) : "<null>"));

  const IList* old_iend = x.iconnection.iend;

  // Initialize state flags
  x.iconnection.iend = NULL;
  x.iconnection.size = 0;
  StarState& state = x.GetState<StarState>();
  state.ok = true;
  state.bad = false;
  state.done = true;
  x.oconnection.Clear();

  if (x.children.empty()) {
    throw FWError("StarRule::Update: StarState " + string(x) + " must have children");
  }

  // Iterate over children, either existing or being created, starting at the
  // updated_child, so long as our last child is complete.
  bool finished = false;
  TreeDS::child_mod_iter child = x.children.begin();
  x.oconnection.ostart = child->oconnection.ostart;
  // Skip ahead to the updated child
  TreeDS* prev_child = NULL;
  if (updated_child) {
    for (child = x.children.begin(); child != x.children.end(); ++child) {
      if (updated_child == &*child) { break; }
      State& istate = child->GetState();
      if (istate.bad || istate.ok || !istate.done) {
        throw FWError("StarRule " + string(*this) + " found 'skippable' child that was not complete");
      }
      if (!child->oconnection.hotlist.empty()) {
        throw FWError("StarRule " + string(*this) + " found 'skippable' child with nonempty hotlist... is that a problem?");
      }
      if (prev_child) {
        if (prev_child->iconnection.iend != child->iconnection.istart) {
          throw FWError("StarRule " + string(*this) + " found iend->istart mismatch; we could correct this, but let's not...");
          //connector.RepositionNode(*child, *prev_child->iconnection.iend);    // Could correct it like this
        }
        if (!prev_child->oconnection.oend ||
              (child->oconnection.ostart &&
                  (prev_child->oconnection.oend->right != child->oconnection.ostart ||
                  child->oconnection.ostart->left != prev_child->oconnection.oend))) {
          throw FWError("StarRule " + string(*this) + " found 'skippable' oend->right <-> ostart->left mismatch; we could correct this, but let's not...");
        }
      }
      x.iconnection.size += child->iconnection.size;
      x.oconnection.oend = child->oconnection.oend;
      x.oconnection.size += child->oconnection.size;
      prev_child = &*child;
    }
  }

  while (!finished) {
    if (child != x.children.end()) {
      // Existing child
      if (prev_child) {
        if (prev_child->iconnection.iend != child->iconnection.istart) {
          throw FWError("StarRule " + string(*this) + " found iend->istart mismatch; we could correct this, but let's not...");
          //connector.RepositionNode(*child, *prev_child->iconnection.iend);    // Could correct it like this
        }
      }
    } else {
      // New child
      std::auto_ptr<TreeDS> newChild(new TreeDS(m_children.at(0).MakeState(), &x));
      x.children.push_back(newChild);
      child = x.children.end() - 1;
      if (prev_child) {
        if (!prev_child->iconnection.iend) {
          throw FWError("StarRule " + string(*this) + " prev child failed to assign its iend");
        }
        connector.RepositionNode(*child, *prev_child->iconnection.iend);
      } else {
        connector.RepositionNode(*child, *x.iconnection.istart);
      }
    }
    x.iconnection.size += child->iconnection.size;
    x.oconnection.oend = child->oconnection.oend;
    x.oconnection.size += child->oconnection.size;
    x.oconnection.hotlist.insert(child->oconnection.hotlist.begin(), child->oconnection.hotlist.end());
    child->oconnection.hotlist.clear();

    // Now check the child's state, and see if we can keep going.
    State& istate = child->GetState();

    if (istate.done) {
      // Link the prev_child's oend and the new child's ostart.
      // prev_child is complete, so it has an oend.  But this child might not
      // be complete so might not have an ostart.
      if (!child->oconnection.ostart) {
        throw FWError("StarRule " + string(*this) + " found done child " + string(*child) + " that does not have an ostart");
      }
      if (prev_child) {
        prev_child->oconnection.oend->right = child->oconnection.ostart;
        child->oconnection.ostart->left = prev_child->oconnection.oend;
      }
    }
    if (istate.bad) {
      m_log.debug("StarRule " + string(*this) + " has gone bad");
      state.ok = false;
      state.bad = true;
      state.done = false;
      x.iconnection.iend = child->iconnection.iend;
      x.oconnection.ostart = NULL;
      x.oconnection.oend = NULL;
      x.oconnection.size = 0;
      // Clear any subsequent children
      for (TreeDS::child_mod_iter i = child; i != x.children.end(); ++i) {
        connector.ClearNode(*i);
      }
      x.children.erase(child, x.children.end());
      finished = true;
    } else if (istate.done && !istate.ok) {
      // Cool, keep going!
    } else if (istate.ok) {
      if (child->iconnection.iend != NULL) {
        throw FWError("StarRule found incomplete inode that is only ok; not allowed");
      } else if (x.iconnection.iend) {
        throw FWError("OrRule reached eoi but tried to set an iend.. silly internal check");
      }
      state.ok = true;
      state.bad = false;
      state.done = false;
      finished = true;
    } else {
      throw FWError("StarRule " + string(*this) + " child is in unexpected state");
    }

    prev_child = &*child;
    ++child;
  }

  m_log.debug("StarRule " + string(*this) + " done update; now has state " + string(x) + " and hotlist size is " + boost::lexical_cast<string>(x.oconnection.hotlist.size()));
  return old_iend != x.iconnection.iend || !x.oconnection.hotlist.empty();
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
