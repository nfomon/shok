// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Seq.h"

#include "Connector.h"
#include "OutputFunc.h"
#include "RestartFunc.h"
#include "SLog.h"
#include "STree.h"

#include <memory>
#include <string>
#include <vector>
using std::auto_ptr;
using std::string;
using std::vector;

using namespace statik;

auto_ptr<Rule> statik::SEQ(const string& name) {
  return auto_ptr<Rule>(new Rule(name,
      MakeRestartFunc_Sequence(),
      MakeComputeFunc_Seq(),
      MakeOutputFunc_Sequence()));
}

auto_ptr<ComputeFunc> statik::MakeComputeFunc_Seq() {
  return auto_ptr<ComputeFunc>(new ComputeFunc_Seq());
}

void ComputeFunc_Seq::operator() (ConnectorAction::Action action, const IList& inode, const STree* initiator) {
  // Process
  g_log.debug() << "Computing Seq at " << *m_node << " with initiator " << (initiator ? string(*initiator) : "<null>");

  if (m_node->children.empty()) {
    throw SError("SeqRule::Update: Seq node " + string(*m_node) + " must have children");
  } else if (m_node->children.size() > m_node->GetRule().GetChildren().size()) {
    throw SError("SeqRule::Update: Seq node " + string(*m_node) + " has more children than the rule");
  }

  // Find the initiator child
  bool foundInitiator = false;
  STree::child_mod_iter child = m_node->children.begin();
  STree* prev_child = NULL;
  unsigned int child_index = 0;
  // TODO eliminate this loop
  while (child != m_node->children.end()) {
    if (initiator == *child) {
      foundInitiator = true;
      break;
    }
    prev_child = *child;
    ++child;
    ++child_index;
  }
  if (!foundInitiator) {
    throw SError("ComputeFunc_Seq at " + string(*m_node) + " provided an initiator which is not a child");
  }

  // What happened to the initiator child?  Cleared / Shrank / Stayed / Grew
  State& state = m_node->GetState();
  const State& childState = (*child)->GetState();
  if ((*child)->IsClear()) {
    g_log.debug() << "ComputeFunc_Seq at " << *m_node << ": Child was cleared, so restarting next";
    STree* next = NULL;
    if (child+1 != m_node->children.end()) {
      next = *(child+1);
    }
    m_node->children.erase(child);
    if (next && prev_child) {
      m_node->GetConnector().Enqueue(ConnectorAction(ConnectorAction::Restart, *next, prev_child->IEnd(), m_node));
    } else if (next && !prev_child) {
      g_log.warning() << "Seq: Not sure we handle this case right.. first child was removed, and it has subsequent child that we are not restarting (here).";
    }
  } else if (!childState.IsComplete()) {
    g_log.debug() << "ComputeFunc_Seq at " << *m_node << ": Child is not complete, so not fixing its next connections";
  } else {
    switch(action) {
    case ConnectorAction::ChildGrow: {
      STree::child_mod_iter next = child+1;
      if (next != m_node->children.end()) {
        g_log.debug() << "ComputeFunc_Seq at " << *m_node << ": Child grew, so clearing and restarting next, which is " << **next;
        // Clear and Restart the next child
        (*next)->ClearNode();
        m_node->GetConnector().Enqueue(ConnectorAction(ConnectorAction::Restart, **next, (*child)->IEnd(), m_node));
      } else if (child_index != m_node->GetRule().GetChildren().size() - 1) {
        g_log.debug() << "ComputeFunc_Seq at " << *m_node << ": Child grew, so creating new next";
        // Create next child
        (void) m_node->GetRule().GetChildren().at(child_index+1)->MakeNode(*m_node, (*child)->IEnd());
      }
    } break;
    case ConnectorAction::ChildShrink: {
      g_log.debug() << "ComputeFunc_Seq at " << *m_node << ": Child shrank, so restarting next without clearing it";
      STree::child_mod_iter next = child+1;
      if (next != m_node->children.end()) {
        // Restart the next child
        m_node->GetConnector().Enqueue(ConnectorAction(ConnectorAction::Restart, **next, (*child)->IEnd(), m_node));
      } else if (child_index != m_node->GetRule().GetChildren().size() - 1) {
        // Create next child
        (void) m_node->GetRule().GetChildren().at(child_index+1)->MakeNode(*m_node, (*child)->IEnd());
        g_log.debug() << "Created next child!";
      }
    } break;
    case ConnectorAction::ChildUpdate: {
      g_log.debug() << "ComputeFunc_Seq at " << *m_node << ": Child update, but did not change size.  Make sure its next connection is correct";
      STree::child_mod_iter next = child+1;
      if (next != m_node->children.end()) {
        if (&(*child)->IEnd() != &(*next)->IStart()) {
          m_node->GetConnector().Enqueue(ConnectorAction(ConnectorAction::Restart, **next, (*child)->IEnd(), m_node));
        }
      } else if (child_index != m_node->GetRule().GetChildren().size() - 1) {
        // Create next child
        (void) m_node->GetRule().GetChildren().at(child_index+1)->MakeNode(*m_node, (*child)->IEnd());
      }
    } break;
    default:
      throw SError("ComputeFunc_Seq at " + string(*m_node) + " received unexpected action " + ConnectorAction::UnMapAction(action));
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
  child_index = 0;
  g_log.debug() << "Finding breach";
  for (STree::child_iter i = m_node->children.begin(); i != m_node->children.end(); ++i) {
    g_log.debug() << "Investigating child " << **i;
    if (!breachChild) {
      isize += (*i)->ISize();
    }
    const State& istate = (*i)->GetState();
    if (istate.IsBad() || istate.IsInit() || (!istate.IsComplete() && child_index < m_node->GetRule().GetChildren().size() - 1)) {
      breachChild = *i;
    }
    if (istate.IsLocked()) {
      lockedChild = *i;
    }
    if (breachChild && lockedChild) {
      break;
    }
    ++child_index;
  }

  if (lockedChild) {
    state.Lock();
  }

  if (breachChild) {
    // We have a breach.  Check it out, it determines our state and IEnd.
    const State& istate = breachChild->GetState();
    g_log.debug() << "Investigating breach child " << *breachChild;
    if (istate.IsInit()) {
      // Cannot set IEnd!
      state.GoBad();
    } else if (istate.IsBad()) {
      m_node->GetIConnection().SetEnd(breachChild->IStart(), isize);
      state.GoBad();
    } else {
      m_node->GetIConnection().SetEnd(breachChild->IEnd(), isize + breachChild->ISize());
      state.GoOK();
    }
  } else {
    // All children are complete until the last observed child.  Determine our
    // state and IEnd based on this last child.
    if (m_node->children.empty()) {
      m_node->ClearNode();    // TODO this might be overkill
    } else {
      const STree& lastChild = *m_node->children.back();
      const State& istate = lastChild.GetState();
      if (istate.IsBad() || istate.IsInit()) {
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

  g_log.debug() << "Computing Seq at " << *m_node << " done update; now has state " << *m_node;
  g_log.debug() << " - and it has istart " << m_node->IStart() << " and iend " << m_node->IEnd();
}
