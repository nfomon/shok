// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Star.h"

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

auto_ptr<Rule> statik::STAR(const string& name) {
  return auto_ptr<Rule>(new Rule(name,
      MakeParseFunc_Star(/*plus*/false),
      MakeOutputFunc_Sequence()));
}

auto_ptr<Rule> statik::PLUS(const string& name) {
  return auto_ptr<Rule>(new Rule(name,
      MakeParseFunc_Star(/*plus*/true),
      MakeOutputFunc_Sequence()));
}

auto_ptr<ParseFunc> statik::MakeParseFunc_Star(bool plus) {
  return auto_ptr<ParseFunc>(new ParseFunc_Star(plus));
}

void ParseFunc_Star::operator() (ParseAction::Action action, const List& inode, const STree* initiator) {
  // Process
  g_log.debug() << "Parsing Star at " << *m_node << " which has " << m_node->children.size() << " children";

  State& state = m_node->GetState();
  if (ParseAction::Restart == action && m_node->children.empty()) {
    if (m_node->GetRule().GetChildren().size() != 1) {
      throw SError("Star rule must have exactly 1 child");
    }
    m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, inode, m_node->children.begin());
    state.GoPending();
    return;
  }

  if (m_node->children.empty()) {
    throw SError("Cannot parse Star node which has no children");
  }

  if (ParseAction::Restart == action) {
    g_log.debug() << "Star restart: comparing first child's IStart against inode " << inode;
    int startCompare = m_node->GetIncParser().INodeCompare((*m_node->children.begin())->IStart(), inode);
    if (startCompare < 0) {
      g_log.info() << "Prepending a new child behind our first child";
      m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, inode, m_node->children.begin());
      state.GoPending();
      return;
    } else if (startCompare > 0) {
      g_log.debug() << "Restart end compare";
      int endCompare = m_node->GetIncParser().INodeCompare(m_node->IEnd(), inode);
      if (endCompare < 0 || (0 == endCompare && !inode.right)) {
        g_log.debug() << "Restarting first child to new, further-ahead location";
        m_node->GetIncParser().Enqueue(ParseAction(ParseAction::Restart, **m_node->children.begin(), inode, m_node));
        state.GoPending();
        return;
      } else {
        g_log.debug() << "Eliminating child that is now passed";
        m_node->children.erase(m_node->children.begin());
        m_node->GetIncParser().Enqueue(ParseAction(ParseAction::Restart, *m_node, inode, m_node));
        state.GoPending();
        return;
      }
    } else {
      g_log.info() << "Star node found Restart=0 action even though we have children.  This is presumably caused by a forced self-update after our first child was deleted so we moved ourselves right.  Go on to re-determine our state.";
      action = ParseAction::ChildUpdate;  // force move-along
      initiator = m_node->children.front();
    }
  }

  if (ParseAction::ChildUpdate != action) {
    throw SError("Star failed to process non-ChildUpdate-action properly");
  }

  // FIXME SLOPPY: erase any clear/pending children. LoL :D
  vector<STree::child_mod_iter> children_to_erase;
  STree::child_mod_iter clear_child = m_node->children.begin();
  for (; clear_child != m_node->children.end(); ++clear_child) {
    if ((*clear_child)->IsClear() || (*clear_child)->GetState().IsPending()) {
      children_to_erase.push_back(clear_child);
    }
  }
  vector<STree::child_mod_iter>::reverse_iterator i = children_to_erase.rbegin();
  for (; i != children_to_erase.rend(); ++i) {
    g_log.debug() << "Star " << *m_node << ": Erasing clear/pending child " << **i;
    m_node->children.erase(*i);
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
    g_log.info() << "ParseFunc_Star at " << *m_node << " provided an initiator which is not a child; presumably we've already dealt with this update";
    return;
  }
  g_log.debug() << "Initiator child: " << **child;

  const State& childState = (*child)->GetState();
  STree::child_mod_iter next = child+1;
  if ((*child)->IsClear()) {
    if (next != m_node->children.end()) {
      if (!prev_child) {
        g_log.debug() << "ParseFunc_Star at " << *m_node << ": First child was cleared, determining what to do...";
        m_node->children.erase(child);
        if (inode.right && inode.right->left != &inode) {
          g_log.info() << "Restarting self at right inode " << *inode.right;
          //g_log.warning() << "Probably shouldn't be doing this here, i.e.  maybe the parent should decide that we should restart at our right inode now that our IStart is gone.  But is top-node behaviour different than subsidiaries?";
          //m_node->GetIncParser().Enqueue(ParseAction(ParseAction::Restart, *m_node, *inode.right, m_node));
          g_log.info() << "Our IStart will be set to the right INode, and we'll come back to Restart.";
          //m_node->GetIConnection().Restart(*inode.right); // not necessary
          m_node->GetIncParser().Enqueue(ParseAction(ParseAction::Restart, *m_node, *inode.right, m_node));
          m_node->GetIncParser().ForceChange(*m_node);
          return;
        } else {
          //g_log.warning() << "ParseFunc_Star at " << *m_node << ": First child was cleared, but it's not as if our first INode was deleted but we can just move forward an INode.  Clearing self.";
          m_node->ClearNode(inode);
          return;
        }
      } else if (prev_child->GetState().IsPending()) {
        throw SError("Star cannot investigate Pending prev child");
      } else if (prev_child->IsClear() || prev_child->GetState().IsPending()) {
        throw SError("Star would look at clear prev_child");
      } else if ((*next)->IsClear() || (*next)->GetState().IsPending()) {
        throw SError("Star would look at clear next child");
        //g_log.info() << "Next child is cleared; enqueueing an update on self to deal with it, followed by a repeat of this update. eeks";
        //m_node->GetIncParser().Enqueue(ParseAction(ParseAction::ChildUpdate, *m_node, inode, *next));
        //m_node->GetIncParser().Enqueue(ParseAction(ParseAction::ChildUpdate, *m_node, inode, *child));
        //return;
      } else {
        // TODO this chunk might be aggressive; we're using prev_child's IEnd even if it's somehow Bad.
        g_log.info() << "Dealing with cleared middle child";
        /*
        if (prev_child->GetState().IsBad()) {
          g_log.warning() << "Using bad prev_child's IEnd";
        }
        */
        if (&prev_child->IEnd() == &(*next)->IStart()) {
          // How can this happen?  It's because we received the update from the
          // cleared child before receiving the prev_child's update.
          g_log.debug() << "ParseFunc_Star at " << *m_node << ": Not-first Child was cleared, but next is already in the right spot";
          m_node->children.erase(child);
          // Keep going to determine our state
        } else {
          g_log.debug() << "Child end compare";
          int childCompare = m_node->GetIncParser().INodeCompare(prev_child->IEnd(), (*next)->IStart());
          if (0 == childCompare) {
            throw SError("ParseFunc_Star: prev->next children are not linked but INodeCompare returned 0");
          } else if (childCompare > 0) {
            // Re-start cleared intermediary child
            g_log.debug() << "ParseFunc_Star at " << *m_node << ": in wake of cleared middle child, replacing it to soak up the middle INodes";
            STree::child_mod_iter pos = child - 1;
            m_node->children.erase(child);
            ++pos;
            (void) m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, prev_child->IEnd(), pos);
            state.GoPending();
            return;
          } else {  // childCompare < 0
            g_log.debug() << "Another child end compare";
            int endCompare = m_node->GetIncParser().INodeCompare(prev_child->IEnd(), (*next)->IEnd());
            if (endCompare < 0) {
              g_log.debug() << "Eliminating next, which is now passed";
              // We do need to enqueue a self-update on a node that will
              // actually exist once these nodes are cleared.
              (*next)->ClearNode(inode);
              m_node->children.erase(next);
              m_node->children.erase(child);
              m_node->GetIncParser().Enqueue(ParseAction(ParseAction::ChildUpdate, *m_node, inode, prev_child));
              state.GoPending();
              return;
            } else {
              g_log.debug() << "Restarting next child to new, further-ahead location (1)";
              m_node->GetIncParser().Enqueue(ParseAction(ParseAction::Restart, **next, prev_child->IEnd(), m_node));
              m_node->children.erase(child);
              state.GoPending();
              return;
            }
          }
        }
      }
    } else {
      if (prev_child) {
        g_log.debug() << "ParseFunc_Star at " << *m_node << ": last child was cleared; erasing it";
        m_node->children.erase(child);
        // Keep going to determine our state
      } else {
        g_log.debug() << "ParseFunc_Star at " << *m_node << ": only child was cleared, so clearing self";
        m_node->ClearNode(inode);
        return;
      }
    }
  } else if (childState.IsPending()) {
    throw SError("Star child should not be Pending");
  } else if (childState.IsOK() || childState.IsDone()) {
    g_log.debug() << "ParseFunc_Star at " << *m_node << ": Child is ok or done but not complete, so not fixing its next connections";
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
      g_log.debug() << "ParseFunc_Star at " << *m_node << ": Last child updated, and is complete.  Creating new next child.";
      (void) m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, (*child)->IEnd());
      state.GoPending();
      return;
    }

    // TODO I'm not sure what to do, if not look at the next child's IStart,
    // even if it's bad.  But I'm not sure this isn't terrible.
    /*
    if ((*next)->GetState().IsBad()) {
      g_log.warning() << "Using bad next child's IStart";
    }
    */
    if (&(*child)->IEnd() == &(*next)->IStart()) {
      g_log.debug() << "ParseFunc_Star at " << *m_node << ": Next child is linked properly, so no changes to children";
      g_log.debug() << " child: " << *child << ":" << **child << " and next: " << *next << ":" << **next;
      g_log.debug() << " child iend: " << (*child)->IEnd() << ":" << &(*child)->IEnd() << " and next istart: " << (*next)->IStart() << ":" << &(*next)->IStart();
    } else {
      int childCompare = m_node->GetIncParser().INodeCompare((*child)->IEnd(), (*next)->IStart());
      if (0 == childCompare) {
        throw SError("ParseFunc_Star: children are not linked but INodeCompare returned 0");
      } else if (childCompare > 0) {
        // If the next child is bad, it's unclear if we want to restart it, or
        // create a new intermediary in-between.  If the node went bad because
        // its IStart was deleted, then childCompare will be > 0 even though
        // that's a bit misleading, but let's err on the side of restarting the
        // bad node until we have experience otherwise.
        if ((*next)->GetState().IsBad()) {
          m_node->GetIncParser().Enqueue(ParseAction(ParseAction::Restart, **next, inode, m_node));
          state.GoPending();
          return;
        } else {
          // Create a new intermediary child
          g_log.debug() << "ParseFunc_Star at " << *m_node << ": Child shrank and is complete, so creating a new intermediary child";
          (void) m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, (*child)->IEnd(), next);
          state.GoPending();
          return;
        }
      } else {  // childCompare < 0
        g_log.debug() << "end-start child compare < 0";
        int endCompare = m_node->GetIncParser().INodeCompare((*child)->IEnd(), (*next)->IEnd());
        if (endCompare < 0) {
          g_log.debug() << "Eliminating next, which is now passed";
          // We must enqueue an update on the child that still exists and still
          // needs to link up to a next node.  It's virtually a child update :)
          (*next)->ClearNode(inode);
          m_node->children.erase(next);
          m_node->GetIncParser().Enqueue(ParseAction(ParseAction::ChildUpdate, *m_node, (*child)->IEnd(), *child));
          state.GoPending();
          return;
        } else {
          g_log.debug() << "Restarting next child to new, further-ahead location (2)";
          m_node->GetIncParser().Enqueue(ParseAction(ParseAction::Restart, **next, (*child)->IEnd(), m_node));
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
      g_log.info() << "Star's breach child is Pending; clearing it and recomputing self";
      for (STree::child_mod_iter i = m_node->children.begin(); i != m_node->children.end(); ++i) {
        if (breachChild == *i) {
          m_node->children.erase(i);
          g_log.debug() << " - erasing the cleared child!";
          state.GoPending();
          m_node->GetIncParser().Enqueue(ParseAction(ParseAction::ChildUpdate, *m_node, inode, m_node));
          return;
        }
      }
      throw SError("Failed to erase the pending child");
    } else if (istate.IsBad()) {
      // last child breached, prev children all complete => complete
      // otherwise, we're bad
      if (m_node->children.empty()) {
        throw SError("Should not be evaluating bad breach child when node has no children");
      }
      if (&*m_node->children.back() == breachChild) {
        if (m_plus && &*m_node->children.front() == breachChild) {
          state.GoBad();
          m_node->GetIConnection().SetEnd(breachChild->IEnd());
        } else {
          state.GoComplete();
          m_node->GetIConnection().SetEnd(breachChild->IEnd());
        }
      } else {
        state.GoBad();
        m_node->GetIConnection().SetEnd(breachChild->IEnd());
      }
    } else if (istate.IsOK()) {
      state.GoOK();
      m_node->GetIConnection().SetEnd(breachChild->IEnd());
    } else if (istate.IsDone()) {
      if (m_plus && &*m_node->children.front() == breachChild) {
        state.GoOK();
        m_node->GetIConnection().SetEnd(breachChild->IEnd());
      } else {
        state.GoDone();
        m_node->GetIConnection().SetEnd(breachChild->IEnd());
      }
    } else {
      throw SError("Star parse found breach in non-breach state");
    }
  } else {
    // All children are complete until the last observed child.  Determine our
    // state and IEnd based on this last child.
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
      m_node->GetIConnection().SetEnd(lastChild.IEnd());
    }
  }

  g_log.debug() << "Parsing Star at " << *m_node << " done update; now has state " << *m_node;
}
