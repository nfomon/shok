// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Connector.h"

#include "Grapher.h"
#include "Hotlist.h"
#include "IList.h"
#include "Rule.h"
#include "SLog.h"
#include "STree.h"

#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
using std::auto_ptr;
using std::map;
using std::set;
using std::string;

using namespace statik;

/* public */

Connector::Connector(const Rule& rule, const string& name, const string& graphdir)
  : m_rule(rule),
    m_root(NULL),
    m_name(name) {
  if (!graphdir.empty()) {
    m_grapher.reset(new Grapher(graphdir, string(name + "_")));
    m_grapher->AddMachine(name, m_rule);
    m_grapher->SaveAndClear();
  }
}

const Hotlist& Connector::GetHotlist() const {
  return m_hotlist;
}

void Connector::ClearHotlist() {
  m_hotlist.Clear();
  m_nodePool.Cleanup();
}

Connector::listener_set Connector::GetListeners(const IList& x) const {
  return m_listeners.GetListeners(&x);
}

void Connector::Insert(const IList& inode) {
  g_log.info() << "Connector " << m_name << ": Inserting IList: " << inode << " with " << (inode.left ? "left" : "no left") << " and " << (inode.right ? "right" : "no right");

  if (!m_root) {
    g_log.debug() << " - No root; initializing the tree root";
    m_root = m_rule.MakeRootNode(*this);
  }
  if (m_root->IsClear()) {
    g_log.debug() << " - Clear root; restarting the tree root";
    Enqueue(ConnectorAction(ConnectorAction::Restart, *m_root, inode));
  } else {
    if (inode.left) {
      g_log.debug() << " - Found left inode " << *inode.left << ": updating listeners of " << inode << "'s left and (if present) right";
      listener_set listeners = m_listeners.GetListeners(inode.left);
      for (listener_iter i = listeners.begin(); i != listeners.end(); ++i) {
        Enqueue(ConnectorAction(ConnectorAction::INodeInsert, **i, inode));
      }
      if (inode.right) {
        listeners = m_listeners.GetListeners(inode.right);
        for (listener_iter i = listeners.begin(); i != listeners.end(); ++i) {
          Enqueue(ConnectorAction(ConnectorAction::INodeInsert, **i, inode));
        }
      }
    } else {
      g_log.debug() << " - No left inode: just restarting the root";
      Enqueue(ConnectorAction(ConnectorAction::Restart, *m_root, inode));
    }
  }

  ProcessActions();

  // Assert that the inode has at least one listener in the updated set.
  if (!m_listeners.HasAnyListeners(&inode)) {
    m_root->GetState().GoBad();
  }
  // Stronger check: every inode has at least one listener :)

  m_hotlist.Accept(m_root->GetOutputFunc().GetHotlist());
  m_root->GetOutputFunc().ClearHotlist();
  g_log.debug() << "Connector: Insert done, hotlist now has size " << m_hotlist.Size();
}

void Connector::Delete(const IList& inode) {
  g_log.info() << "Deleting list inode " << inode;

  if (!m_root) {
    throw SError("Connector " + m_name + ": cannot delete " + string(inode) + " when the root has not been initialized");
  }

  if (!inode.left && !inode.right) {
    // FIXME if this is necessary, it means we're doing something wrong.
    // Hotlist is already ordered.  Anything in it, even old stuff we've
    // declared as "deleted" since, should be valid memory we can look at.
    /*
    for (OutputFunc::emitting_iter i = m_root->GetOutputFunc().Emitting().begin(); i != m_root->GetOutputFunc().Emitting().end(); ++i) {
      m_hotlist.Delete(**i);
    }
    */
    m_root->ClearNode();
  } else {
    if (inode.left) {
      listener_set listeners = m_listeners.GetListeners(inode.left);
      for (listener_iter i = listeners.begin(); i != listeners.end(); ++i) {
        Enqueue(ConnectorAction(ConnectorAction::INodeDelete, **i, inode));
      }
    }
    listener_set listeners = m_listeners.GetListeners(&inode);
    for (listener_iter i = listeners.begin(); i != listeners.end(); ++i) {
      Enqueue(ConnectorAction(ConnectorAction::INodeDelete, **i, inode));
    }
  }

  m_listeners.RemoveAllListeners(&inode);

  m_hotlist.Accept(m_root->GetOutputFunc().GetHotlist());
  m_root->GetOutputFunc().ClearHotlist();
  g_log.debug() << "Connector: Delete done, hotlist now has size " << m_hotlist.Size();
}

void Connector::Update(const IList& inode) {
  g_log.info() << "Connector " << m_name << ": Updating listeners of inode " << inode;
  listener_set listeners = m_listeners.GetListeners(&inode);
  for (listener_iter i = listeners.begin(); i != listeners.end(); ++i) {
    Enqueue(ConnectorAction(ConnectorAction::INodeUpdate, **i, inode));
  }
}

void Connector::UpdateWithHotlist(const Hotlist::hotlist_vec& hotlist) {
  g_log.info() << "Updating Connector " << m_name << " with hotlist of size " << hotlist.size();
  for (Hotlist::hotlist_iter i = hotlist.begin(); i != hotlist.end(); ++i) {
    switch (i->second) {
    case Hotlist::OP_INSERT:
      Insert(*i->first);
      break;
    case Hotlist::OP_DELETE:
      Delete(*i->first);
      break;
    case Hotlist::OP_UPDATE:
      Update(*i->first);
      break;
    default:
      throw SError("Cannot update with hotlist with unknown hot operation");
    }
  }
  g_log.info() << "Done updating Connector " << m_name << " with hotlist that had size " << hotlist.size();
  if (!m_hotlist.IsEmpty()) {
    DrawGraph(*m_root);
  }
}

void Connector::Enqueue(ConnectorAction action) {
  int depth = action.node->depth;
  action_map::iterator ai = m_actions_by_depth.find(depth);
  if (m_actions_by_depth.end() == ai) {
    action_queue actions;
    actions.push_back(action);
    m_actions_by_depth.insert(std::make_pair(depth, actions));
  } else {
    ai->second.push_back(action);
  }
}

void Connector::ClearNode(STree& x) {
  m_listeners.RemoveAllListenings(&x);
  if (&x != m_root) {
    m_nodePool.Unlink(x);
  }
}

void Connector::Listen(STree& x, const IList& inode) {
  if (m_listeners.IsListening(&inode, &x)) {
    g_log.info() << "Connector: " << x << " is already listening to " << inode;
  } else {
    g_log.info() << "Connector: " << x << " will listen to " << inode;
    m_listeners.AddListener(&inode, &x);
  }
}

void Connector::Unlisten(STree& x, const IList& inode) {
  g_log.info() << "Connector: " << x << " will NOT listen to " << inode;
  m_listeners.RemoveListener(&inode, &x);
}

void Connector::UnlistenAll(STree& x) {
  g_log.info() << "Connector " << m_name << ": Unlistening all from node " << x;
  m_listeners.RemoveAllListenings(&x);
}

STree* Connector::OwnNode(auto_ptr<STree> node) {
  g_log.debug() << "Connector " << m_name << ": Owning node " << string(*node);
  return m_nodePool.Insert(node);
}

const IList* Connector::GetFirstONode() const {
  if (!m_root) {
    return NULL;
  }
  return m_root->GetOutputFunc().OStart();
}

void Connector::DrawGraph(const STree& onode, const IList* inode) {
  if (!m_grapher.get()) {
    return;
  }
  if (!m_root) {
    return;
  }
  m_grapher->AddIList(m_name, m_root->IStart(), m_name + " input");
  m_grapher->AddSTree(m_name, *m_root, m_name + " output");
  if (m_root->GetOutputFunc().OStart()) {
    m_grapher->AddOList(m_name, *m_root->GetOutputFunc().OStart(), m_name + " olist");
  g_log.debug() << " Done drawing OList";
  }
  m_grapher->AddIListeners(m_name, *this, m_root->IStart());
  if (inode) {
    m_grapher->Signal(m_name, &onode);
    m_grapher->Signal(m_name, inode);
  } else {
    m_grapher->Signal(m_name, &onode, true);
  }
  m_grapher->AddHotlist(m_name, m_hotlist.GetHotlist());
  m_grapher->SaveAndClear();
}

/* private */

const STree& Connector::GetRoot() const {
  if (!m_root) {
    throw SError("Connector " + m_name + "; cannot get root node before it has been initialized");
  }
  return *m_root;
}

/*
void Connector::UpdateListeners(const IList& inode, bool updateLeft, bool updateThis, bool updateRight) {
  if (updateLeft && inode.left) {
    // TODO Unnecessary copy
    listener_set listeners = m_listeners.GetListeners(inode.left);
    for (listener_iter i = listeners.begin(); i != listeners.end(); ++i) {
      changes_by_depth[(*i)->depth].insert(*i);
    }
  }

  if (updateThis && (!updateLeft || inode.left != &inode)) {
    // TODO Unnecessary copy
    listener_set listeners = m_listeners.GetListeners(&inode);
    for (listener_iter i = listeners.begin(); i != listeners.end(); ++i) {
      changes_by_depth[(*i)->depth].insert(*i);
    }
  }

  if (updateRight && (!updateLeft || inode.left != inode.right)
                  && (!updateThis || &inode != inode.right)) {
    // TODO Unnecessary copy
    listener_set listeners = m_listeners.GetListeners(inode.right);
    for (listener_iter i = listeners.begin(); i != listeners.end(); ++i) {
      changes_by_depth[(*i)->depth].insert(*i);
    }
  }

  if (changes_by_depth.empty()) {
    g_log.info() << "Connector: No listeners of " << inode << " to update (left: " + (inode.left ? string(*inode.left) : "<null>") << "; right: " << (inode.right ? string(*inode.right) : "<null>") << ")";
  }

  while (!changes_by_depth.empty()) {
    // Update the deepest depth of our changeset
    int depth = changes_by_depth.rbegin()->first;
    change_set changes = changes_by_depth.rbegin()->second;
    g_log.debug() << "Connector: Updating changes at depth " << depth;
    for (change_iter i = changes.begin(); i != changes.end(); ++i) {
      bool changed = false;
      // If the node's IStart() has been deleted, let's clear it.
      if (&(*i)->IStart() == &inode && ((inode.left && inode.left->right != &inode) || (inode.right && inode.right->left != &inode))) {
        g_log.debug() << "UpdateListeners: node " << **i << " lost its IStart, so clearing it";
        (*i)->ClearNode();
        changed = true;
      } else {
        g_log.debug() << "UpdateListeners: Computing node " << **i << " with IStart " << (*i)->IStart() << " in response to INode " << inode;
        changed = (*i)->ComputeNode();
      }
      if (changed) {
        STree* parent = (*i)->GetParent();
        if (parent) {
          changes_by_depth[parent->depth].insert(parent);
        }
      }
    }
    changes_by_depth.erase(depth);
  }
}
*/

void Connector::ProcessActions() {
  while (!m_actions_by_depth.empty()) {
    STree::depth_t depth = m_actions_by_depth.rbegin()->first;
    action_queue& actions = m_actions_by_depth.rbegin()->second;
    g_log.debug() << "Connector " << m_name << ": Applying actions at depth " << depth;
    while (m_actions_by_depth.rbegin()->first == depth && !actions.empty()) {
      ConnectorAction* a = &actions.front();
      actions.pop_front();
      switch (a->action) {
      case ConnectorAction::Restart:
        a->node->RestartNode(*a->inode);
        break;
      case ConnectorAction::INodeUpdate:
        a->node->ComputeNode(*a->inode, a->initiator);
        break;
      case ConnectorAction::INodeInsert:
        a->node->ComputeNode(*a->inode, a->initiator);
        break;
      case ConnectorAction::INodeDelete:
        a->node->ComputeNode(*a->inode, a->initiator);
        break;
      default:
        throw SError("ProcessActions: unknown action");
      }
    }
    if (m_actions_by_depth.at(depth).empty()) {
      m_actions_by_depth.erase(depth);
    }
  }
}
