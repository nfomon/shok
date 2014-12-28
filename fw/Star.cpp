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
  State& state = x.GetState();
  state.Clear();

  if (x.children.empty()) {
    throw FWError("StarRule::Update: Star node " + string(x) + " must have children");
  }
  x.GetIConnection().SetStart(x.children.at(0).IStart());

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
        if (!child->IStart().left) {
          throw FWError("Star " + string(*this) + " child " + string(*child) + " has istart at the start of input, but it is not our first child");
        }
        if (&prev_child->IEnd() != &child->IStart()) {
          m_log.info("Star " + string(*this) + " child " + string(*child) + " needs to be repositioned to the node after the prev child's end");
          connector.RepositionNode(*child, prev_child->IEnd());
        }
      }
    } else {
      // New child
      const IList* newIStart = &x.IStart();
      if (prev_child) {
        newIStart = &prev_child->IEnd();
      }
      if (!newIStart) {
        throw FWError("Star " + string(*this) + (prev_child ? (" with prev child " + string(*prev_child)) : " with no prev child") + " failed to find new istart for new child");
      }
      FWTree* newChild = AddChildToNode(x, m_children.at(0), *newIStart);
      connector.RepositionNode(*newChild, *newIStart);
      child = x.children.end() - 1;
    }
    x.GetIConnection().SetEnd(child->IEnd());

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
      if (child->IEnd().right) {
        throw FWError("Star found incomplete inode that is only ok; not allowed");
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
  m_log.debug(" - and it has istart " + string(x.IStart()) + " and iend " + string(x.IEnd()));
}
