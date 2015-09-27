// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Seq.h"

#include "IncParser.h"
#include "OutputFunc.h"
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
      MakeParseFunc_Seq(),
      MakeOutputFunc_Sequence()));
}

auto_ptr<ParseFunc> statik::MakeParseFunc_Seq() {
  return auto_ptr<ParseFunc>(new ParseFunc_Seq());
}

void ParseFunc_Seq::operator() (ParseAction::Action action, const List& inode, const STree* initiator) {
  // Process
  g_log.debug() << "Parsing Seq at " << *m_node << " with initiator " << (initiator ? initiator->Print() : "<null>") << " and " << m_node->children.size() << " children";

  State& state = m_node->GetState();
  if (ParseAction::Restart == action) {
    if (m_node->children.empty()) {
      if (m_node->GetRule().GetChildren().empty()) {
        throw SError("Star rule must have at least one child");
      }
      m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, inode, m_node->children.begin());
      state.GoPending();
      return;
    } else if (m_node->children.size() > m_node->GetRule().GetChildren().size()) {
      throw SError("SeqRule::Update: Seq node " + m_node->Print() + " has more children than the rule");
    }
    m_node->GetIncParser().Enqueue(ParseAction(ParseAction::Restart, *m_node->children.front(), inode, m_node));
    state.GoPending();
    return;
  }
  if (ParseAction::ChildUpdate != action) {
    throw SError("Seq failed to process non-ChildUpdate-action properly");
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
    throw SError("ParseFunc_Seq at " + m_node->Print() + " provided an initiator which is not a child");
  }
  g_log.debug() << "Initiator child: " << **child;

  const State& childState = (*child)->GetState();
  STree::child_mod_iter next = child+1;
  if ((*child)->IsClear()) {
    g_log.debug() << "ParseFunc_Seq at " << *m_node << ": Child was cleared, so restarting next";
    if (next != m_node->children.end()) {
      if (!prev_child) {
        m_node->ClearNode(inode);
        return;
      } else if (prev_child->GetState().IsPending()) {
        throw SError("Cannot investigate pending child");
      }
      /*
      if (prev_child->GetState().IsBad()) {
        g_log.warning() << "Seq: in response to cleared child, is using bad prev_child " << *prev_child << "'s IEnd";
      }
      */
      m_node->GetIncParser().Enqueue(ParseAction(ParseAction::Restart, **next, prev_child->IEnd(), m_node));
      state.GoPending();
      return;
    } 
    m_node->children.erase(child);
    // Keep going to determine our state
  } else if (childState.IsPending()) {
    throw SError("Seq child should not be Pending");
  } else if (childState.IsOK() || childState.IsDone()) {
    g_log.debug() << "ParseFunc_Seq at " << *m_node << ": Child is ok or done but not complete, so not fixing its next connections";
    if (next != m_node->children.end() && !(*child)->IEnd().right) {
      g_log.debug() << " - but we're at end of input, so clear all subsequent children";
      for (STree::child_mod_iter i = next; i != m_node->children.end(); ++i) {
        (*i)->ClearNode(inode);
      }
      m_node->children.erase(next, m_node->children.end());
    }
    // Keep going to determine our state
  } else if (childState.IsComplete()) {
    g_log.debug() << "ParseFunc_Seq at " << *m_node << ": Child is complete.  Check on its next connections.";
    if (next != m_node->children.end()) {
      if (&(*child)->IEnd() != &(*next)->IStart()) {
        m_node->GetIncParser().Enqueue(ParseAction(ParseAction::Restart, **next, (*child)->IEnd(), m_node));
        state.GoPending();
        return;
      }
    } else if (child_index != m_node->GetRule().GetChildren().size() - 1) {
      // Create next child
      (void) m_node->GetRule().GetChildren().at(child_index+1)->MakeNode(*m_node, (*child)->IEnd());
      state.GoPending();
      return;
    }
/*
    switch(action) {
    case ParseAction::ChildGrow: {
      if (next != m_node->children.end()) {
        if (&(*child)->IEnd() == &(*next)->IStart()) {
          g_log.debug() << "ParseFunc_Seq at " << *m_node << ": Child grew, but next node is already in the right spot";
        } else {
          // Clear and Restart the next child
          g_log.debug() << "ParseFunc_Seq at " << *m_node << ": Child grew, so clearing and restarting next, which is " << **next;
          (*next)->ClearNode(inode);
          m_node->GetIncParser().Enqueue(ParseAction(ParseAction::Restart, **next, (*child)->IEnd(), m_node));
          state.GoOK();
          return;
        }
      } else if (child_index != m_node->GetRule().GetChildren().size() - 1) {
        g_log.debug() << "ParseFunc_Seq at " << *m_node << ": Child grew, so creating new next";
        // Create next child
        (void) m_node->GetRule().GetChildren().at(child_index+1)->MakeNode(*m_node, (*child)->IEnd());
        state.GoOK();
        return;
      }
    } break;
    case ParseAction::ChildShrink: {
      g_log.debug() << "ParseFunc_Seq at " << *m_node << ": Child shrank, so restarting next without clearing it";
      if (next != m_node->children.end()) {
        // Restart the next child
        m_node->GetIncParser().Enqueue(ParseAction(ParseAction::Restart, **next, (*child)->IEnd(), m_node));
        state.GoOK();
        return;
      } else if (child_index != m_node->GetRule().GetChildren().size() - 1) {
        // Create next child
        (void) m_node->GetRule().GetChildren().at(child_index+1)->MakeNode(*m_node, (*child)->IEnd());
        state.GoOK();
        return;
      }
    } break;
*/
  } else if (childState.IsBad()) {
    g_log.debug() << "Seq: Child is bad, so leaving subsequent connections alone; let it breach";
  } else {
    throw SError("Seq: Child update but is in unknown state");
  }

  // Compute
  // Find the first breach and locked children
  // TODO eliminate this loop, just cache the index of the first
  // breached/locked children as sequence state and
  // update during Process as appropriate.
  const STree* breachChild = NULL;
  const STree* lockedChild = NULL;
  for (STree::child_iter i = m_node->children.begin(); i != m_node->children.end(); ++i) {
    const State& istate = (*i)->GetState();
    if (!breachChild && !istate.IsComplete()) {
      breachChild = *i;
    }
    if (!lockedChild && istate.IsLocked()) {
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
      throw SError("Seq's breach child is Pending");
    } else if (istate.IsBad()) {
      state.GoBad();
      m_node->GetIConnection().SetEnd(breachChild->IEnd());
    } else if (istate.IsOK()) {
      state.GoOK();
      m_node->GetIConnection().SetEnd(breachChild->IEnd());
    } else if (istate.IsDone()) {
      state.GoDone();
      m_node->GetIConnection().SetEnd(breachChild->IEnd());
    } else {
      throw SError("Seq parse found breach in non-breach state");
    }
  } else {
    // All children are complete until the last observed child.  Determine our
    // state and IEnd based on this last child.
    if (m_node->children.empty()) {
      m_node->ClearNode(inode);
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
      m_node->GetIConnection().SetEnd(lastChild.IEnd());
    }
  }

  g_log.debug() << "Parsing Seq at " << *m_node << " done update; now has state " << *m_node;
}
