// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Star.h"

#include "Connector.h"
#include "FWLog.h"
#include "FWTree.h"
#include "OutputFunc.h"
#include "RestartFunc.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <memory>
#include <string>
#include <vector>
using std::auto_ptr;
using std::string;
using std::vector;

using namespace fw;

auto_ptr<Rule> fw::STAR(const string& name) {
  return auto_ptr<Rule>(new Rule(name,
      MakeRestartFunc_FirstChildOfNode(),
      MakeComputeFunc_Star(),
      MakeOutputFunc_Sequence()));
}

auto_ptr<ComputeFunc> fw::MakeComputeFunc_Star() {
  return auto_ptr<ComputeFunc>(new ComputeFunc_Star());
}

void ComputeFunc_Star::operator() () {
  g_log.debug() << "Computing Star at " << *m_node;

  // Initialize state flags
  State& state = m_node->GetState();
  state.Clear();

  if (m_node->children.empty()) {
    throw FWError("Computing Star at " + string(*m_node) + " must have children");
  }
  m_node->GetIConnection().Restart(m_node->children.at(0).IStart());

  // Iterate over children, either existing or being created, so long as our
  // last child is complete.
  bool finished = false;
  FWTree::child_mod_iter child = m_node->children.begin();
  const FWTree* prev_child = NULL;
  bool wasComplete = true;    // Star is "complete" matching absolutely nothing
  while (!finished) {
    if (child != m_node->children.end()) {
      // Existing child
      if (prev_child) {
        if (!child->IStart().left) {
          throw FWError("Computing Star at " + string(*m_node) + " child " + string(*child) + " has istart at the start of input, but it is not our first child");
        }
        if (&prev_child->IEnd() != &child->IStart()) {
          g_log.info() << "Computing Star at " << *m_node << " child " << *child << " needs to be repositioned to the node after the prev child's end";
          child->RestartNode(prev_child->IEnd());
        }
      }
    } else {
      // New child
      const IList* newIStart = &m_node->IStart();
      if (prev_child) {
        newIStart = &prev_child->IEnd();
      }
      if (!newIStart) {
        throw FWError("Computing Star at " + string(*m_node) + (prev_child ? (" with prev child " + string(*prev_child)) : " with no prev child") + " failed to find new istart for new child");
      }
      m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, *newIStart);
      child = m_node->children.end() - 1;
    }
    m_node->GetIConnection().SetEnd(child->IEnd());

    // Now check the child's state, and see if we can keep going.
    State& istate = child->GetState();

    if (istate.IsLocked()) {
      state.Lock();
    }

    if (istate.IsBad()) {
      if (wasComplete) {
        g_log.debug() << "Computing Star at " << *m_node << " has gone bad but its last child was complete, so now it's complete";
        state.GoComplete();
      } else {
        g_log.debug() << "Computing Star at " << *m_node << " has gone bad";
        state.GoBad();
      }
      // Clear any subsequent children
      for (FWTree::child_mod_iter i = child+1; i != m_node->children.end(); ++i) {
        i->ClearNode();
      }
      m_node->children.erase(child+1, m_node->children.end());
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
      // Clear any subsequent children
      for (FWTree::child_mod_iter i = child+1; i != m_node->children.end(); ++i) {
        i->ClearNode();
      }
      m_node->children.erase(child+1, m_node->children.end());
      finished = true;
    } else {
      throw FWError("Computing Star at " + string(*m_node) + " child is in unexpected state");
    }

    prev_child = &*child;
    ++child;
  }

  if (!prev_child) {
    throw FWError("Computing Star at " + string(*m_node) + " should have assigned a previous child at some point");
  }

  g_log.debug() << "Computing Star at " << *m_node << " done update; now has state " << *m_node;
  g_log.debug() << " - and it has istart " << m_node->IStart() << " and iend " << m_node->IEnd();
}
