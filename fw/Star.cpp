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

void StarRule::Reposition(Connector& connector, FWTree& x, const IList& inode) const {
  m_log.debug("Repositioning StarRule " + string(*this) + " at " + string(x) + " with " + string(inode));
  RepositionFirstChildOfNode(connector, x, inode);
  Update(connector, x, &x.children.at(0));
}

void StarRule::Update(Connector& connector, FWTree& x, const FWTree* updated_child) const {
  m_log.debug("Updating StarRule " + string(*this) + " at " + string(x) + " with child " + (updated_child ? string(*updated_child) : "<null>"));

  // Initialize state flags
  x.iconnection.iend = NULL;
  x.iconnection.size = 0;
  StarState& state = x.GetState<StarState>();
  state.Clear();

  if (x.children.empty()) {
    throw FWError("StarRule::Update: Star node " + string(x) + " must have children");
  }

  // Iterate over children, either existing or being created, starting at the
  // updated_child, so long as our last child is complete.
  bool finished = false;
  FWTree::child_mod_iter child = x.children.begin();
  // Skip ahead to the updated child
  FWTree* prev_child = NULL;
  if (updated_child) {
    for (child = x.children.begin(); child != x.children.end(); ++child) {
      if (updated_child == &*child) { break; }
      State& istate = child->GetState();
      if (istate.IsLocked()) {
        state.Lock();
      }
      if (!istate.IsComplete()) {
        throw FWError("StarRule " + string(*this) + " found 'skippable' child that was not complete");
      }
      if (prev_child) {
        if (prev_child->iconnection.iend != child->iconnection.istart) {
          throw FWError("StarRule " + string(*this) + " found iend->istart mismatch; we could correct this, but let's not...");
          //connector.RepositionNode(*child, *prev_child->iconnection.iend);    // Could correct it like this
        }
      }
      x.iconnection.size += child->iconnection.size;
      prev_child = &*child;
    }
  }

  bool wasComplete = false;
  while (!finished) {
    bool isNewChild = false;
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
      isNewChild = true;
      std::auto_ptr<FWTree> newChild(new FWTree(m_log, m_children.at(0).MakeState(), &x));
      x.children.push_back(newChild);
      child = x.children.end() - 1;
      if (prev_child) {
        if (!prev_child->iconnection.iend) {
          throw FWError("StarRule " + string(*this) + " prev child " + string(*prev_child) + " failed to assign its iend");
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

    if (istate.IsEmitting()) {
      x.GetOConnection<OConnectionSequence>().AddNextChild(*child, isNewChild);
    }

    if (istate.IsBad()) {
      if (wasComplete) {
        m_log.debug("StarRule " + string(*this) + " has gone bad but its last child was complete, so now it's complete");
        state.GoComplete();
      } else {
        m_log.debug("StarRule " + string(*this) + " has gone bad");
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
        throw FWError("StarRule found incomplete inode that is only ok; not allowed");
      } else if (x.iconnection.iend) {
        throw FWError("OrRule reached eoi but tried to set an iend.. silly internal check");
      }
      if (istate.IsDone()) {
        state.GoDone();
      } else {
        state.GoOK();
      }
      finished = true;
    } else {
      throw FWError("StarRule " + string(*this) + " child is in unexpected state");
    }

    prev_child = &*child;
    ++child;
  }

  if (!prev_child) {
    throw FWError("StarRule " + string(*this) + " should have assigned a previous child at some point");
  }

  m_log.debug("StarRule " + string(*this) + " done update; now has state " + string(x) + " and hotlist size is " + boost::lexical_cast<string>(x.GetOConnection().GetHotlist().size()));
}

std::auto_ptr<State> StarRule::MakeState() const {
  return std::auto_ptr<State>(new StarState(*this));
}

std::auto_ptr<OConnection> StarRule::MakeOConnection(const FWTree& x) const {
  return std::auto_ptr<OConnection>(new OConnectionSequence(m_log, x));
}
