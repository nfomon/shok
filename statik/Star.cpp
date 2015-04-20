// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Star.h"

#include "Connector.h"
#include "OutputFunc.h"
#include "RestartFunc.h"
#include "SLog.h"
#include "STree.h"

#include <memory>
#include <set>
#include <string>
#include <vector>
using std::auto_ptr;
using std::set;
using std::string;
using std::vector;

using namespace statik;

auto_ptr<Rule> statik::STAR(const string& name) {
  return auto_ptr<Rule>(new Rule(name,
      MakeRestartFunc_Sequence(),
      MakeComputeFunc_Star(),
      MakeOutputFunc_Sequence()));
}

auto_ptr<ComputeFunc> statik::MakeComputeFunc_Star() {
  return auto_ptr<ComputeFunc>(new ComputeFunc_Star());
}

void ComputeFunc_Star::operator() (ConnectorAction::Action action, const IList& inode, const STree* initiator) {
  // Process
  g_log.debug() << "Computing Star at " << *m_node << " which has " << m_node->children.size() << " children";

  if (m_node->children.empty()) {
    throw SError("Computing Star at " + string(*m_node) + " must have children");
  }

  // Find the initiator child
  bool foundInitiator = false;
  STree::child_mod_iter child = m_node->children.begin();
  STree* prev_child = NULL;
  // TODO eliminate this loop
  while (child != m_node->children.end()) {
    if (initiator == *child) {
      foundInitiator = true;
      break;
    }
    prev_child = *child;
    ++child;
  }
  if (!foundInitiator) {
    throw SError("ComputeFunc_Star at " + string(*m_node) + " provided an initiator which is not a child");
  }

  // What happened to the initiator child?  Cleared / Shrank / Stayed / Grew
  State& state = m_node->GetState();
  const State& childState = (*child)->GetState();
  STree::child_mod_iter next = child+1;
  if ((*child)->IsClear()) {
    g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Child was cleared, so restarting next";
    if (next != m_node->children.end()) {
      if (!prev_child) {
        throw SError("Star: Not sure we handle this case right.. first child was removed, and it has subsequent child that we are not restarting (here).");
      } else if (prev_child->GetState().IsBadOrInit()) {
        throw SError("Seq initiator child cleared but prev_child exists and is bad or init.  What to do about next children -- clear them all?");
      }
      m_node->GetConnector().Enqueue(ConnectorAction(ConnectorAction::Restart, **next, prev_child->IEnd(), m_node));
      state.GoOK();
      return;
    }
    // This is safe down here because the above just enqueues stuff, nothing else.
    m_node->children.erase(child);
  } else if (!childState.IsComplete()) {
    g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Child is not complete, so not fixing its next connections";
    if (!(*child)->GetState().IsBadOrInit() && next != m_node->children.end() && !(*child)->IEnd().right) {
      g_log.debug() << " - but we're at end of input, so clear all subsequent children";
      for (STree::child_mod_iter i = next; i != m_node->children.end(); ++i) {
        (*i)->ClearNode();
      }
      m_node->children.erase(next, m_node->children.end());
    }
  } else {
    switch(action) {
    case ConnectorAction::ChildGrow: {
      if (next != m_node->children.end()) {
        if (&(*child)->IEnd() == &(*next)->IStart()) {
          g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Child grew, but next node is already in the right spot";
        } else {
          // Clear and Restart the next child
          g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Child grew, so clearing and restarting next, which is " << **next;
          (*next)->ClearNode();
          m_node->GetConnector().Enqueue(ConnectorAction(ConnectorAction::Restart, **next, (*child)->IEnd(), m_node));
          state.GoOK();
          return;
        }
      } else {
        // Create next child
        g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Child grew, and no next child, so creating new next child";
        (void) m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, (*child)->IEnd());
        state.GoOK();
        return;
      }
    } break;
    case ConnectorAction::ChildShrink: {
      g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Child shrank, so restarting next without clearing it";
      if (next != m_node->children.end()) {
        // Restart the next child
        m_node->GetConnector().Enqueue(ConnectorAction(ConnectorAction::Restart, **next, (*child)->IEnd(), m_node));
        state.GoOK();
        return;
      } else {
        // Create next child
        (void) m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, (*child)->IEnd());
        state.GoOK();
        return;
      }
    } break;
    case ConnectorAction::ChildUpdate: {
      g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Child update, but did not change size.  Make sure its next connection is correct";
      if (next != m_node->children.end()) {
        if (&(*child)->IEnd() != &(*next)->IStart()) {
          //m_node->GetConnector().Enqueue(ConnectorAction(ConnectorAction::Restart, **next, (*child)->IEnd(), m_node));
          (void) m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, (*child)->IEnd(), next);
          state.GoOK();
          return;
        }
      } else {
        // Create next child
        (void) m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, (*child)->IEnd());
        state.GoOK();
        return;
      }
    } break;
    default:
      throw SError("ComputeFunc_Star at " + string(*m_node) + " received unexpected action " + ConnectorAction::UnMapAction(action));
    }
  }

  // Compute
  // Find the first breach and locked children
  // TODO eliminate this loop, just cache the index of the first
  // breached/locked children as sequence state (and vector of ISizes!) and
  // update during Process as appropriate.
  const STree* breachChild = NULL;
  const STree* lockedChild = NULL;
  size_t isize = 0;
  for (STree::child_iter i = m_node->children.begin(); i != m_node->children.end(); ++i) {
    if (!breachChild) {
      isize += (*i)->ISize();
    }
    const State& istate = (*i)->GetState();
    if (istate.IsBadOrInit() || !istate.IsComplete()) {
      breachChild = *i;
    }
    if (istate.IsLocked()) {
      lockedChild = *i;
    }
    if (breachChild && lockedChild) {
      break;
    }
  }

  if (lockedChild) {
    state.Lock();
  }

  if (breachChild) {
    // We have a breach.  Check it out, it determines our state and IEnd.
    const State& istate = breachChild->GetState();
    g_log.debug() << "Investigating breach child " << *breachChild;
    if (istate.IsBadOrInit()) {
      // Cannot set IEnd!
      state.GoBad();
    } else if (istate.IsOK()) {
      state.GoOK();
      m_node->GetIConnection().SetEnd(breachChild->IEnd(), isize + breachChild->ISize());
    } else if (istate.IsDone()) {
      state.GoDone();
      m_node->GetIConnection().SetEnd(breachChild->IEnd(), isize + breachChild->ISize());
    } else {
      throw SError("Seq compute found breach in non-breach state");
    }
  } else {
    // All children are complete until the last observed child.  Determine our
    // state and IEnd based on this last child.
    g_log.debug() << " - no breach child";
    if (m_node->children.empty()) {
      m_node->ClearNode();    // TODO this might be overkill
    } else {
      const STree& lastChild = *m_node->children.back();
      const State& istate = lastChild.GetState();
      if (istate.IsBadOrInit()) {
        throw SError("Found bad last child, but breach child was not set");
      } else if (istate.IsComplete()) {
        state.GoComplete();
      } else if (istate.IsDone()) {
        state.GoDone();
      } else if (istate.IsOK()) {
        state.GoOK();
      } else {
        throw SError("Last child has unknown state");
      }
      m_node->GetIConnection().SetEnd(lastChild.IEnd(), isize);
    }
  }

  g_log.debug() << "Computing Star at " << *m_node << " done update; now has state " << *m_node;
}
