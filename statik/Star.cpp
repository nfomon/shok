// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Star.h"

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

auto_ptr<Rule> statik::STAR(const string& name) {
  return auto_ptr<Rule>(new Rule(name,
      MakeRestartFunc_FirstChildOfNode(),
      MakeComputeFunc_Star(),
      MakeOutputFunc_Sequence()));
}

auto_ptr<ComputeFunc> statik::MakeComputeFunc_Star() {
  return auto_ptr<ComputeFunc>(new ComputeFunc_Star());
}

void ComputeFunc_Star::operator() () {
  g_log.debug() << "Computing Star at " << *m_node << " which has " << m_node->children.size() << " children";

  // Initialize state flags
  State& state = m_node->GetState();
  state.Clear();

  if (m_node->children.empty()) {
    throw SError("Computing Star at " + string(*m_node) + " must have children");
  }
  m_node->GetIConnection().Restart(m_node->children.at(0)->IStart());

  // Iterate over children, either existing or being created.  If our last
  // child is complete, we can create a new child after it.
  bool finished = false;
  STree::child_mod_iter child = m_node->children.begin();
  const STree* prev_child = NULL;
  bool wasComplete = true;    // Star is "complete" matching absolutely nothing
  while (!finished) {
    bool makeNode = (m_node->children.end() == child);
    if (!makeNode) {
      // Existing child
      if (prev_child) {
        if (!(*child)->IStart().left) {
          throw SError("Computing Star at " + string(*m_node) + " child " + string(**child) + " has istart at the start of input, but it is not our first child");
        }
        if (&prev_child->IEnd() != &(*child)->IStart()) {
          //g_log.info() << "Computing Star at " << *m_node << " child " << **child << " needs to be repositioned to the node after the prev child's end";
          g_log.info() << "Computing Star at " << *m_node << " child " << **child << " is not positioned to the node after the prev child's end.  Determining what to do in-between.";
          const IList* istart = &prev_child->IEnd();
          g_log.debug() << "Starting node is: " << *istart;
          // TODO check if the prev child CONTAINS the (*child->IStart() node.  If it does, then we should clear out this child and all after.
          // There's not a pressing need for this afaik; just saves some work, presumably.

          // Go forward across all the children, to see if someone's INode list
          // already CONTAINS this inode.  If it does, delete all the children
          // up to that point, and restart that node if necessary.  Otherwise,
          // insert a new child right here, leaving all the subsequent ones.
          bool foundMatch = false;
          for (STree::child_mod_iter nextChild = child; nextChild != m_node->children.end(); ++nextChild) {
            bool done = false;
            g_log.debug() << " - - investigating child " << **nextChild << " with istart=" << (*nextChild)->IStart();
            for (const IList* inode = &(*nextChild)->IStart(); inode != NULL && (inode != &(*nextChild)->IEnd() || !inode->right); inode = inode->right) {
              g_log.debug() << " - - - investigating INode " << *inode;
              if (istart == inode) {
                g_log.debug() << " - - found a match!";
                foundMatch = true;

                // Clear all children between child and nextChild
                prev_child->GetOutputFunc().OEnd()->right = (*nextChild)->GetOutputFunc().OStart();
                (*nextChild)->GetOutputFunc().OStart()->left = prev_child->GetOutputFunc().OEnd();
                vector<STree::child_mod_iter> toClear;
                for (STree::child_mod_iter i = child; i != nextChild; ++i) {
                  toClear.push_back(i);
                }
                --child;    // don't get clobbered by child deletes
                g_log.debug() << " - Child is hiding at " << **child;
                g_log.debug() << toClear.size() << " children to delete";
                for (vector<STree::child_mod_iter>::const_iterator j = toClear.begin(); j != toClear.end(); ++j) {
                  g_log.debug() << "Clearing " << ***j;
                  (**j)->ClearNode();
                  g_log.debug() << " Done clear";
                  g_log.debug();
                  m_node->children.erase(*j);
                }
                ++child;    // now go my child to where we want you to be
                g_log.debug() << " - Child is now " << **child;
                g_log.debug() << " - And we have " << m_node->children.size() << " children";

                // Restart the node if the INode was in its middle somewhere
                if (inode != &(*child)->IStart()) {
                  g_log.debug() << " - - Restarting this child to this inode";
                  g_log.debug() << "INode is " << *inode << " and child start is " << (*child)->IStart();
                  (*child)->RestartNode(*istart);
                }

                done = true;
                break;
              }
            }
            if (done) { break; }
          }

          if (!foundMatch) {
            g_log.debug() << " - found no matches.";
            makeNode = true;
          }
        }
      }
    }
    if (makeNode) {
      // New child
      const IList* newIStart = &m_node->IStart();
      if (prev_child) {
        newIStart = &prev_child->IEnd();
      }
      if (!newIStart) {
        throw SError("Computing Star at " + string(*m_node) + (prev_child ? (" with prev child " + string(*prev_child)) : " with no prev child") + " failed to find new istart for new child");
      }
      // TODO let's not do this scan; instead, get back the iterator we want from MakeNode()?
      STree* newChild = m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, *newIStart, child);
      for (child = m_node->children.begin(); child != m_node->children.end(); ++child) {
        if (*child == newChild) {
          break;
        }
      }
      if (child == m_node->children.end()) {
        throw SError("Computing Star at " + string(*m_node) + " broke its new child insertion");
      }
    }
    m_node->GetIConnection().SetEnd((*child)->IEnd());

    // Now check the child's state, and see if we can keep going.
    State& istate = (*child)->GetState();

    if (istate.IsLocked()) {
      state.Lock();
    }

    if (istate.IsBad()) {
      if (wasComplete) {
        if (state.IsBad()) {
          g_log.debug() << "Computing Star at " << *m_node << " would go complete but is already bad";
        } else {
          g_log.debug() << "Computing Star at " << *m_node << " has gone bad at " << **child << " but its previous child (or empty-state) was complete, so now it's complete";
          state.GoComplete();
        }
      } else {
        g_log.debug() << "Computing Star at " << *m_node << " has gone bad at " << **child;
        state.GoBad();
      }
      // Leave any subsequent children be.
/*
      // Clear any subsequent children
      // First fix their oconnections
      if ((*child)->GetOutputFunc().OEnd()) {
        if (m_node->children.back()->GetOutputFunc().OEnd() && m_node->children.back()->GetOutputFunc().OEnd()->right) {
          m_node->children.back()->GetOutputFunc().OEnd()->right->left = NULL;
        }
        (*child)->GetOutputFunc().OEnd()->right = NULL;
      }
      for (STree::child_mod_iter i = child+1; i != m_node->children.end(); ++i) {
        (*i)->ClearNode();
      }
      m_node->children.erase(child+1, m_node->children.end());
*/
      finished = true;
    } else if (istate.IsComplete()) {
      wasComplete = true;
      // Cool, keep going!
    } else if (istate.IsAccepting()) {
      wasComplete = false;
      if ((*child)->IEnd().right) {
        throw SError("Star found incomplete inode that is only ok; not allowed");
      }
      if (istate.IsDone()) {
        if (state.IsBad()) {
          g_log.debug() << "Computing Star at " << *m_node << " would go done but is already bad";
        } else {
          state.GoDone();
        }
      } else {
        if (state.IsBad()) {
          g_log.debug() << "Computing Star at " << *m_node << " would go OK but is already bad";
        } else {
          state.GoOK();
        }
      }
      // Clear any subsequent children
      // First fix their oconnections
/*
      if ((*child)->GetOutputFunc().OEnd()) {
        if (m_node->children.back()->GetOutputFunc().OEnd() && m_node->children.back()->GetOutputFunc().OEnd()->right) {
          m_node->children.back()->GetOutputFunc().OEnd()->right->left = NULL;
        }
        (*child)->GetOutputFunc().OEnd()->right = NULL;
      }
      for (STree::child_mod_iter i = child+1; i != m_node->children.end(); ++i) {
        (*i)->ClearNode();
      }
      m_node->children.erase(child+1, m_node->children.end());
*/
      finished = true;
    } else {
      throw SError("Computing Star at " + string(*m_node) + " child is in unexpected state");
    }

    prev_child = *child;
    ++child;
  }

  if (!prev_child) {
    throw SError("Computing Star at " + string(*m_node) + " should have assigned a previous child at some point");
  }

  g_log.debug() << "Computing Star at " << *m_node << " done update; now has state " << *m_node;
  g_log.debug() << " - and it has istart " << m_node->IStart() << " and iend " << m_node->IEnd();
}
