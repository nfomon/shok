// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Star.h"

#include "Connector.h"
#include "OutputFunc.h"
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
      MakeComputeFunc_Star(),
      MakeOutputFunc_Sequence()));
}

auto_ptr<ComputeFunc> statik::MakeComputeFunc_Star() {
  return auto_ptr<ComputeFunc>(new ComputeFunc_Star());
}

void ComputeFunc_Star::operator() (ConnectorAction::Action action, const IList& inode, const STree* initiator, int resize) {
  // Process
  g_log.debug() << "Computing Star at " << *m_node << " which has " << m_node->children.size() << " children";

  State& state = m_node->GetState();
  if (m_node->children.empty()) {
    m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, inode, m_node->children.begin());
    state.GoPending();
    return;
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
    g_log.info() << "ComputeFunc_Star at " << *m_node << " provided an initiator which is not a child; presumably we've already dealt with this";
    return;
  }

  // What happened to the initiator child?  Cleared / Pending / Incomplete / Shrank / Stayed / Grew
  const State& childState = (*child)->GetState();
  STree::child_mod_iter next = child+1;
  if ((*child)->IsClear()) {
    if (next != m_node->children.end()) {
      if (!prev_child) {
        g_log.debug() << "ComputeFunc_Star at " << *m_node << ": First Child was cleared, so hack-restarting self";
        m_node->GetIConnection().Restart((*next)->IStart());
        m_node->children.erase(child);
        // Keep going to determine our state
      } else if (prev_child->GetState().IsPending()) {
        throw SError("Cannot investigate Pending prev child");
      } else if (prev_child->IsClear() || prev_child->GetState().IsPending()) {
        throw SError("Would look at clear prev_child");
      } else if ((*next)->IsClear() || (*next)->GetState().IsPending()) {
        throw SError("Would look at clear next");
      } else {
        if (prev_child->GetState().IsBad()) {
          g_log.warning() << "Using bad prev_child's IEnd";
        }
        // TODO this chunk might be aggressive; we're using prev_child's IEnd even if it's somehow Bad.
        if (&prev_child->IEnd() == &(*next)->IStart()) {
          // How can this happen?  It's because we received the update from the
          // cleared child before receiving the prev_child's update.
          g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Not-first Child was cleared, but next is already in the right spot";
          m_node->children.erase(child);
          // Keep going to determine our state
        } else {
          m_node->GetConnector().Enqueue(ConnectorAction(ConnectorAction::Restart, **next, prev_child->IEnd(), /*FIXME*/0, m_node));
          m_node->children.erase(child);
          state.GoPending();
          return;
        }
      }
    } else {
      if (prev_child) {
        g_log.debug() << "ComputeFunc_Star at " << *m_node << ": last child was cleared; erasing";
        m_node->children.erase(child);
        // Keep going to determine our state
      } else {
        g_log.debug() << "ComputeFunc_Star at " << *m_node << ": only child was cleared, so clearing self";
        m_node->ClearNode(inode);
        return;
      }
    }
  } else if (childState.IsPending()) {
    throw SError("Star child should not be Pending");
  } else if (childState.IsOK() || childState.IsDone()) {
    g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Child is ok or done but not complete, so not fixing its next connections";
    if (next != m_node->children.end() && !(*child)->IEnd().right) {
      g_log.debug() << " - but we're at end of input, so clear all subsequent children";
      for (STree::child_mod_iter i = next; i != m_node->children.end(); ++i) {
        (*i)->ClearNode(inode);
      }
      m_node->children.erase(next, m_node->children.end());
    }
    // Keep going to determine our state
  } else if (childState.IsComplete()) {
    switch(action) {
/*
    case ConnectorAction::ChildGrow: {
      if (next != m_node->children.end()) {
        if (&(*child)->IEnd() == &(*next)->IStart()) {
          g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Child grew and is complete, but next child is already in the right spot";
          // Keep going to determine our state
        } else {
          // FIXME WHAT was the difference between "Clear and Restart" vs just "Restart"?? Because I'm pretty sure that was important, and I'm pretty sure it can't exist anymore.  Instead of Clearing, let's try just Restarting here...
          // Clear and Restart the next child
          g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Child grew and is complete, so clearing and restarting next, which is " << **next;
          //(*next)->ClearNode(inode);  // NOPE lol, then Restarting it would crash! I could clear and make a new node; is that what I would want?  Why?
          m_node->GetConnector().Enqueue(ConnectorAction(ConnectorAction::Restart, **next, (*child)->IEnd(), m_node));
          state.GoPending();
          return;
        }
      } else {
        // Create next child
        g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Last child grew and is complete, so creating new next child";
        (void) m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, (*child)->IEnd());
        state.GoPending();
        return;
      }
    } break;
    case ConnectorAction::ChildShrink: {
      if (next != m_node->children.end()) {
        if (&(*child)->IEnd() == &(*next)->IStart()) {
          g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Child shrank and is complete, but next child is already in the right spot";
          // Keep going to determine our state
        } else {
          // Restart the next child
          g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Child shrank and is complete, so restarting next without clearing it, which is " << **next;
          m_node->GetConnector().Enqueue(ConnectorAction(ConnectorAction::Restart, **next, (*child)->IEnd(), m_node));
          state.GoPending();
          return;
        }
      } else {
        // Create next child
        g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Last child shrank and is complete, so creating new next child";
        (void) m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, (*child)->IEnd());
        state.GoPending();
        return;
      }
    } break;
*/
    case ConnectorAction::ChildUpdate: {
      if (next != m_node->children.end()) {
        if (&(*child)->IEnd() == &(*next)->IStart()) {
          g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Child updated and is complete, but did not change size, but next child is already in the right spot";
          // Keep going to determine our state
        } else {
          g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Child updated and is complete, but did not change size.  Creating new intermediary node -- I think that's right?";
          //m_node->GetConnector().Enqueue(ConnectorAction(ConnectorAction::Restart, **next, (*child)->IEnd(), m_node));
          (void) m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, (*child)->IEnd(), next);
          state.GoPending();
          return;
        }
      } else {
        // Create next child
        g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Last child update and is complete, but did not change size.  Creating new next child.";
        (void) m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, (*child)->IEnd());
        state.GoPending();
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
    if (!istate.IsComplete()) {
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
    if (istate.IsPending()) {
      throw SError("Star's breach child is Pending");
    } else if (istate.IsBad()) {
      state.GoBad();
      m_node->GetIConnection().SetEnd(breachChild->IEnd(), isize + breachChild->ISize());
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
      throw SError("Star no breach; how did all my children get empty?");
    } else {
      const STree& lastChild = *m_node->children.back();
      const State& istate = lastChild.GetState();
      if (istate.IsPending()) {
        throw SError("Found pending last child! Furthermore, breach child was not set!");
      } else if (istate.IsBad()) {
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
