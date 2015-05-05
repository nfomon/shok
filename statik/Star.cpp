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
  if (ConnectorAction::Restart == action && 0 == resize) {
    if (m_node->children.empty()) {
      m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, inode, m_node->children.begin());
      state.GoPending();
      return;
    } else {
      throw SError("Cannot resize Star by 0 when it already has children; or maybe just not implemented.  Clear it first?");
    }
  }

  if (m_node->children.empty()) {
    throw SError("Cannot compute Star node which has no children");
  }

  if (ConnectorAction::Restart == action) {
    if (resize < 0) {
      g_log.info() << "Prepending a new child behind our first child";
      m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, inode, m_node->children.begin());
      state.GoPending();
      return;
    } else if (resize > 0) {
      int child_size = (*m_node->children.begin())->ISize();
      if (child_size < resize) {
        g_log.debug() << "Eliminating child that is now passed";
        m_node->children.erase(m_node->children.begin());
        m_node->GetConnector().Enqueue(ConnectorAction(ConnectorAction::Restart, *m_node, inode, resize-child_size, m_node));
        state.GoPending();
        return;
      } else {
        g_log.debug() << "Restarting first child to new, further-ahead location";
        m_node->GetConnector().Enqueue(ConnectorAction(ConnectorAction::Restart, **m_node->children.begin(), inode, child_size-resize, m_node));
        state.GoPending();
        return;
      }
    } else {
      throw SError("Star node found Restart=0 action unexpectedly");
    }
  }

  if (ConnectorAction::ChildUpdate != action) {
    throw SError("Star failed to process non-ChildUpdate-action properly");
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
    g_log.info() << "ComputeFunc_Star at " << *m_node << " provided an initiator which is not a child; presumably we've already dealt with this update";
    return;
  }

  const State& childState = (*child)->GetState();
  STree::child_mod_iter next = child+1;
  if ((*child)->IsClear()) {
    if (next != m_node->children.end()) {
      // resize param is the size of the cleared child
      if (!prev_child) {
        g_log.debug() << "ComputeFunc_Star at " << *m_node << ": First Child was cleared, so restarting self";
        m_node->GetConnector().Enqueue(ConnectorAction(ConnectorAction::Restart, *m_node, inode, resize, m_node));
        state.GoPending();
        return;
      } else if (prev_child->GetState().IsPending()) {
        throw SError("Cannot investigate Pending prev child");
      } else if (prev_child->IsClear() || prev_child->GetState().IsPending()) {
        throw SError("Would look at clear prev_child");
      } else if ((*next)->IsClear() || (*next)->GetState().IsPending()) {
        throw SError("Would look at clear next");
      } else {
        g_log.info() << "Dealing with cleared middle child";
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
          m_node->GetConnector().Enqueue(ConnectorAction(ConnectorAction::Restart, **next, prev_child->IEnd(), resize, m_node));
          m_node->children.erase(child);
          state.GoPending();
          return;
        }
      }
    } else {
      if (prev_child) {
        g_log.debug() << "ComputeFunc_Star at " << *m_node << ": last child was cleared; erasing it";
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
    if (m_node->children.end() == next) {
      // Create new next child
      g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Last child updated, and is complete.  Creating new next child.";
      (void) m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, (*child)->IEnd());
      state.GoPending();
      return;
    }

    if (&(*child)->IEnd() == &(*next)->IStart()) {
      g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Next child is linked properly, so no changes to children";
    } else {
      if (0 == resize) {
        g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Child is complete but stayed same size, so restarting next, which is " << **next;
        // FIXME this might be a hack
        if (&inode == &(*child)->IEnd()) {
          g_log.debug() << " -- hack inserted case";
          m_node->GetConnector().Enqueue(ConnectorAction(action, *m_node, inode, resize+1, *child));
          state.GoPending();
          return;
        } else if (&inode == &(*next)->IStart()) {
          g_log.debug() << " -- hack deleted case";
          m_node->GetConnector().Enqueue(ConnectorAction(ConnectorAction::Restart, **next, inode, 1, m_node));
          state.GoPending();
          return;
        } else {
          // Weird case: child is complete, resize=0, but nodes are not
          // connected right.  This probably happened when we faked resize=0
          // from a bad child that turned complete.  So create a new
          // intermediary node, assuming that the next child comes later.
          g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Weird case; making new intermediary node";
          (void) m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, (*child)->IEnd(), next);
          state.GoPending();
          return;
        }
      } else if (resize < 0) {
        // Create a new intermediary child
        g_log.debug() << "ComputeFunc_Star at " << *m_node << ": Child shrank and is complete, so creating a new intermediary child";
        (void) m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, (*child)->IEnd(), next);
        state.GoPending();
        return;
      } else {  // resize > 0
        // Eliminate nodes until we get to the resize amount of size change.
        // Then restart that node forward if it's not in the right place.
        int child_size = (*child)->ISize();
        if (child_size < resize) {
          g_log.debug() << "Eliminating child that is now passed";
          // TODO this will cause an automatic self-update, right?  And we'll come back here and keep clearing as far as necessary?
          (*child)->ClearNode(inode);
          m_node->children.erase(child);
          state.GoPending();
          return;
        } else {
          g_log.debug() << "Restarting next child to new, further-ahead location";
          m_node->GetConnector().Enqueue(ConnectorAction(ConnectorAction::Restart, **next, inode, child_size-resize, m_node));
          state.GoPending();
          return;
        }
      }
    }
  } else if (childState.IsBad()) {
    g_log.debug() << "Child is bad, so leaving subsequent connections alone; let it breach";
  } else {
    throw SError("Child update but is in unknown state");
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
