// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Seq.h"

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

void SeqRule::Update(FWTree& x) const {
  m_log.debug("Updating Seq " + string(*this) + " at " + string(x));

  // Initialize state flags
  State& state = x.GetState();
  state.Clear();

  if (x.children.empty()) {
    throw FWError("SeqRule::Update: Seq node " + string(x) + " must have children");
  } else if (x.children.size() > m_children.size()) {
    throw FWError("SeqRule::Update: Seq node " + string(x) + " has more children than the rule");
  }
  x.GetIConnection().Restart(x.children.at(0).IStart());

  // Iterate over node children, either existing or being created, so long as
  // our last child is complete.
  bool finished = false;
  unsigned int child_index = 0;
  FWTree::child_mod_iter child = x.children.begin();
  FWTree* prev_child = NULL;
  while (!finished) {
    if (child_index >= m_children.size()) {
      throw FWError("Seq " + string(*this) + " evaluated too many children beyond the rule");
    }
    if (child != x.children.end()) {
      // Existing child
      if (prev_child) {
        if (!child->IStart().left) {
          throw FWError("Seq " + string(*this) + " child " + string(*child) + " has istart at the start of input, but it's not our first child");
        }
        if (&prev_child->IEnd() != &child->IStart()) {
          m_log.info("Seq " + string(*this) + " child " + string(*child) + " needs to be repositioned to the node after the prev child's end");
          child->RestartNode(prev_child->IEnd());
        }
      }
    } else if (x.children.size() > m_children.size()) {
      throw FWError("SeqRule::Update: Seq node " + string(x) + " has more children than the rule");
    } else if (child_index >= m_children.size()) {
      throw FWError("SeqRule::Update: child index >= Rule size");
    } else {
      // New child
      const IList* newIStart = &x.IStart();
      if (prev_child) {
        newIStart = &prev_child->IEnd();
      }
      if (!newIStart) {
        throw FWError("Seq " + string(*this) + (prev_child ? (" with prev child " + string(*prev_child)) : " with no prev child") + " failed to find new istart for new child");
      }
      (void) m_children.at(child_index).MakeNode(x, *newIStart);
      child = x.children.end() - 1;
    }
    x.GetIConnection().SetEnd(child->IEnd());

    // Now check the child's state, and see if we can keep going.
    State& istate = child->GetState();

    if (istate.IsLocked()) {
      state.Lock();
    }

    if (istate.IsBad()) {
      m_log.debug("Seq " + string(*this) + " has gone bad");
      state.GoBad();
      // Clear any subsequent children
      for (FWTree::child_mod_iter i = child+1; i != x.children.end(); ++i) {
        i->ClearNode();
      }
      x.children.erase(child+1, x.children.end());
      finished = true;
    } else if (istate.IsComplete()) {
      // Are we complete at the end of our sequence?
      if (child_index == m_children.size() - 1) {
        state.GoComplete();
        finished = true;
      } else {
        // Cool, keep going!
      }
    } else if (istate.IsAccepting()) {
      if (child->IEnd().right) {
        throw FWError("Seq found incomplete inode that is only ok; not allowed");
      }
      if (istate.IsDone() && child_index == m_children.size() - 1) {
        state.GoDone();
      } else {
        state.GoOK();
      }
      finished = true;
    } else {
      throw FWError("Seq " + string(*this) + " child is in unexpected state");
    }

    prev_child = &*child;
    ++child;
    ++child_index;
  }

  if (!prev_child) {
    throw FWError("Seq " + string(*this) + " should have assigned a previous child at some point");
  }

  m_log.debug("Seq " + string(*this) + " done update; now has state " + string(x));
  m_log.debug(" - and it has istart " + string(x.IStart()) + " and iend " + string(x.IEnd()));
}
