// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Star.h"

#include "Connector.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <memory>
#include <string>
#include <vector>
using std::auto_ptr;
using std::string;
using std::vector;

using namespace fw;

void StarRule::Reposition(Connector& connector, FWTree& x, const IList& inode) const {
  m_log.debug("Repositioning Star " + string(*this) + " at " + string(x) + " with " + string(inode));
  RepositionFirstChildOfNode(connector, x, inode);
}

void StarRule::Update(Connector& connector, FWTree& x) const {
  m_log.debug("Updating Star " + string(*this) + " at " + string(x));

  // Initialize state flags
  x.iconnection.iend = NULL;
  x.iconnection.size = 0;
  State& state = x.GetState();
  state.Clear();

  if (x.children.empty()) {
    throw FWError("StarRule::Update: Star node " + string(x) + " must have children");
  }

  // Iterate over children, either existing or being created, so long as our
  // last child is complete.
  bool finished = false;
  FWTree::child_mod_iter child = x.children.begin();
  const FWTree* prev_child = NULL;
  bool wasComplete = false;
  while (!finished) {
    if (child != x.children.end()) {
      // Existing child
      if (prev_child) {
        if (prev_child->iconnection.iend != child->iconnection.istart) {
          m_log.info("Star " + string(*this) + " child " + string(*child) + " needs to be repositioned to the prev child's end");
          connector.RepositionNode(*child, *prev_child->iconnection.iend);
        }
      }
    } else {
      // New child
      x.children.push_back(auto_ptr<FWTree>(new FWTree(m_children.at(0), &x)));
      child = x.children.end() - 1;
      if (prev_child) {
        if (!prev_child->iconnection.iend) {
          throw FWError("Star " + string(*this) + " prev child " + string(*prev_child) + " failed to assign its iend");
        }
        connector.RepositionNode(*child, *prev_child->iconnection.iend);
      } else {
        connector.RepositionNode(*child, *x.iconnection.istart);
      }
    }
    x.iconnection.size += child->iconnection.size;

    // Now check the child's state, and see if we can keep going.
    State& istate = child->GetState();

    if (istate.IsLocked()) {
      state.Lock();
    }

    if (istate.IsBad()) {
      if (wasComplete) {
        m_log.debug("Star " + string(*this) + " has gone bad but its last child was complete, so now it's complete");
        state.GoComplete();
      } else {
        m_log.debug("Star " + string(*this) + " has gone bad");
        state.GoBad();
      }
      x.iconnection.iend = child->iconnection.iend;
      // Clear any subsequent children
      for (FWTree::child_mod_iter i = child+1; i != x.children.end(); ++i) {
        connector.ClearNode(*i);
      }
      x.children.erase(child+1, x.children.end());
      finished = true;
    } else if (istate.IsComplete()) {
      wasComplete = true;
      // Cool, keep going!
    } else if (istate.IsAccepting()) {
      wasComplete = false;
      if (child->iconnection.iend != NULL) {
        throw FWError("Star found incomplete inode that is only ok; not allowed");
      } else if (x.iconnection.iend) {
        throw FWError("Star reached eoi but tried to set an iend.. silly internal check");
      }
      if (istate.IsDone()) {
        state.GoDone();
      } else {
        state.GoOK();
      }
      finished = true;
    } else {
      throw FWError("Star " + string(*this) + " child is in unexpected state");
    }

    prev_child = &*child;
    ++child;
  }

  if (!prev_child) {
    throw FWError("Star " + string(*this) + " should have assigned a previous child at some point");
  }

  m_log.debug("Star " + string(*this) + " done update; now has state " + string(x));
}
