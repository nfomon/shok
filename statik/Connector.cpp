// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Connector.h"

#include "Grapher.h"
#include "Hotlist.h"
#include "IList.h"
#include "Rule.h"
#include "SLog.h"
#include "STree.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <map>
#include <set>
#include <string>
using std::map;
using std::set;
using std::string;

using namespace statik;

/* public */

Connector::Connector(const Rule& rule, const string& name, const string& graphdir)
  : m_rule(rule),
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
}

void Connector::Insert(const IList& inode) {
  g_log.info() << "Connector " << m_name << ": Inserting IList: " << inode;

  if (!m_root.get()) {
    g_log.debug() << " - No root; initializing the tree root";
    m_root = m_rule.MakeRootNode(*this);
    m_root->RestartNode(inode);
  } else {
    if (inode.left) {
      g_log.debug() << " - Found left inode: updating listeners of " << inode << "'s left and (if present) right";
      UpdateListeners(inode, true);
    } else {
      // Just reposition the root.
      g_log.debug() << " - No left inode: just repositioning the root";
      m_root->RestartNode(inode);
    }
  }

  // Assert that the inode has at least one listener in the updated set.
  if (!m_listeners.HasAnyListeners(&inode)) {
    m_root->GetState().GoBad();
  }
  // Stronger check: every inode has at least one listener :)

  m_hotlist.Accept(m_root->GetOutputFunc().GetHotlist());
  g_log.debug() << "Connector: Insert done, hotlist now has size " << boost::lexical_cast<string>(m_hotlist.Size());
}

void Connector::Delete(const IList& inode) {
  g_log.info() << "Deleting list inode " << inode;

  if (!m_root.get()) {
    throw SError("Connector " + m_name + ": cannot delete " + string(inode) + " when the root has not been initialized");
  }

  m_listeners.RemoveAllListeners(&inode);

  if (!inode.left && !inode.right) {
    m_root->ClearNode();
  } else if (!inode.left) {
    m_root->RestartNode(*inode.right);
  } else {
    UpdateListeners(inode, true);
  }

  m_hotlist.Accept(m_root->GetOutputFunc().GetHotlist());
  g_log.debug() << "Connector: Delete done, hotlist now has size " << boost::lexical_cast<string>(m_hotlist.Size());
}

void Connector::UpdateWithHotlist(const Hotlist::hotlist_vec& hotlist) {
  for (Hotlist::hotlist_iter i = hotlist.begin(); i != hotlist.end(); ++i) {
    switch (i->second) {
    case Hotlist::OP_INSERT:
      Insert(*i->first);
      break;
    case Hotlist::OP_DELETE:
      Delete(*i->first);
      break;
    case Hotlist::OP_UPDATE:
      UpdateListeners(*i->first, false);
      break;
    default:
      throw SError("Cannot update with hotlist with unknown hot operation");
    }
  }
}

void Connector::ClearNode(STree& x) {
  m_listeners.RemoveAllListenings(&x);
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

/* private */

const STree& Connector::GetRoot() const {
  if (!m_root.get()) {
    throw SError("Connector " + m_name + "; cannot get root node before it has been initialized");
  }
  return *m_root.get();
}

void Connector::UpdateListeners(const IList& inode, bool updateNeighbourListeners) {
  typedef set<STree*> change_set;
  typedef change_set::const_iterator change_iter;
  typedef map<STree::depth_t, change_set> change_map;
  change_map changes_by_depth;

  if (updateNeighbourListeners) {
    if (!inode.left) {
      throw SError("Cannot update listeners of inode " + string(inode) + " with nothing to its left");
    }

    // TODO Unnecessary copy
    listener_set left_listeners = m_listeners.GetListeners(inode.left);
    for (listener_iter i = left_listeners.begin();
         i != left_listeners.end(); ++i) {
      changes_by_depth[(*i)->depth].insert(*i);
    }

    // If inode.right and inode.right != inode.left, merge the left and right
    // listeners, depth-ordered.
    if (inode.right && inode.right != inode.left) {
      // TODO Unnecessary copy
      listener_set right_listeners = m_listeners.GetListeners(inode.right);
      for (listener_iter i = right_listeners.begin();
           i != right_listeners.end(); ++i) {
        changes_by_depth[(*i)->depth].insert(*i);
      }
    }
  } else {
    // TODO Unnecessary copy
    listener_set listeners = m_listeners.GetListeners(&inode);
    for (listener_iter i = listeners.begin(); i != listeners.end(); ++i) {
      changes_by_depth[(*i)->depth].insert(*i);
    }
  }

  if (changes_by_depth.empty()) {
    g_log.info() << "Connector: No " << (updateNeighbourListeners ? "neighbouring" : "direct") << " listeners of " << inode << " (left: " + (inode.left ? string(*inode.left) : "<null>") << "; right: " << (inode.right ? string(*inode.right) : "<null>") << ") to update.";
  }

  while (!changes_by_depth.empty()) {
    // Update the deepest depth of our changeset
    int depth = changes_by_depth.rbegin()->first;
    change_set changes = changes_by_depth.rbegin()->second;
    g_log.debug() << "Connector: Updating changes at depth " << lexical_cast<string>(depth);
    for (change_iter i = changes.begin(); i != changes.end(); ++i) {
      bool changed = (*i)->ComputeNode();
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

void Connector::DrawGraph(const STree& onode, const IList* inode) {
  if (!m_grapher.get()) {
    return;
  }
  if (!m_root.get()) {
    return;
  }
  m_grapher->AddIList(m_name, m_root->IStart(), m_name + " input");
  m_grapher->AddOTree(m_name, *m_root.get(), m_name + " output");
  m_grapher->AddIListeners(m_name, *this, m_root->IStart());
  if (inode) {
    m_grapher->Signal(m_name, &onode);
    m_grapher->Signal(m_name, inode);
  } else {
    m_grapher->Signal(m_name, &onode, true);
  }
  m_grapher->SaveAndClear();
}
