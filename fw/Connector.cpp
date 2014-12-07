// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Connector.h"

#include "DS.h"
#include "Grapher.h"
#include "Rule.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <set>
#include <string>
#include <utility>
using std::make_pair;
using std::multiset;
using std::pair;
using std::string;

using namespace fw;

/* public */

Connector::Connector(Log& log, const Rule& rule, const std::string& name, Grapher* grapher)
  : m_log(log),
    m_root(rule.MakeState(), NULL),
    m_name(name),
    m_grapher(grapher),
    m_istart(NULL) {
}

void Connector::Insert(const IList& inode) {
  m_log.info("Connector: Inserting IList: " + string(inode));

  if (!m_istart) {
    m_istart = &inode;
  }

  if (inode.left) {
    m_log.debug(" - Found left inode: updating listeners of " + string(inode) + "'s left and (if present) right");
    UpdateListeners(inode);
  } else {
    // Just reposition the root.
    m_log.debug(" - No left inode: just repositioning the root");
    RepositionNode(m_root, inode);
  }

  // Assert that the inode has at least one listener in the updated set.
  if (!m_listeners.HasAnyListeners(&inode)) {
    State& state = m_root.GetState();
    state.GoBad();
  }
  // Stronger check: every inode has at least one listener :)

  m_oconnection = m_root.oconnection;
  m_log.debug("Connector: Insert done, hotlist now has size " + boost::lexical_cast<string>(m_oconnection.hotlist.size()));
}

void Connector::Delete(const IList& inode) {
  m_log.info("Deleting list inode " + string(inode));

  m_listeners.RemoveAllListeners(&inode);

  if (!inode.left && !inode.right) {
    ClearNode(m_root);
    m_istart = NULL;
  } else {
    UpdateListeners(inode);
  }

  m_oconnection = m_root.oconnection;
}

void Connector::UpdateWithHotlist(const Hotlist& hotlist) {
  for (Hotlist_iter i = hotlist.begin(); i != hotlist.end(); ++i) {
    switch (i->second) {
    case OP_INSERT:
      Insert(*i->first);
      break;
    case OP_DELETE:
      Delete(*i->first);
      break;
    default:
      throw FWError("Cannot update with hotlist with unknown hot operation");
    }
  }
}

void Connector::RepositionNode(TreeDS& x, const IList& inode) {
  m_log.info("Connector: Repositioning " + std::string(x) + " with inode " + std::string(inode));
  if (x.iconnection.istart == &inode) {
    m_log.info(" - already at the right position; skipping");
    return;
  }
  DrawGraph(x, &inode);
  State& state = x.GetState();
  state.rule.Reposition(*this, x, inode);
}

// Recalculate state based on a change to a child.  Child could be NULL
// meaning the update is called by a direct-subscription.
bool Connector::UpdateNode(TreeDS& x, const TreeDS* child) {
  m_log.info("Connector: Updating " + std::string(x) + " with child " + (child ? std::string(*child) : "<null>"));
  State& state = x.GetState();
  bool r = state.rule.Update(*this, x, child);
  DrawGraph(x);
  return r;
}

void Connector::ClearNode(TreeDS& x) {
  m_log.info("Connector: Clearing " + std::string(x));
  for (TreeDS::child_mod_iter i = x.children.begin();
       i != x.children.end(); ++i) {
    ClearNode(*i);
  }
  m_listeners.RemoveAllListenings(&x);
  x.Clear();
}

void Connector::Listen(TreeDS& x, const IList& inode) {
  m_log.info("Connector: " + std::string(x) + " will listen to " + std::string(inode));
  m_listeners.AddListener(&inode, &x);
}

void Connector::Unlisten(TreeDS& x, const IList& inode) {
  m_log.info("Connector: " + std::string(x) + " will NOT listen to " + std::string(inode));
  m_listeners.RemoveListener(&inode, &x);
}

/* private */

void Connector::UpdateListeners(const IList& inode) {
  typedef std::pair<TreeDS*, const TreeDS*> change_pair;
  typedef std::vector<change_pair> change_vec;
  typedef change_vec::const_iterator change_iter;
  typedef std::map<TreeDS::depth_t, change_vec> change_map;
  change_map changes_by_depth;

  if (!inode.left) {
    throw FWError("Cannot update listeners of inode " + string(inode) + " with nothing to its left");
  }

  // TODO Unnecessary copy
  listener_set left_listeners = m_listeners.GetListeners(inode.left);
  for (listener_iter i = left_listeners.begin();
       i != left_listeners.end(); ++i) {
    changes_by_depth[(*i)->depth].push_back(change_pair(*i, NULL));
  }

  // If inode.right and inode.right != inode.left, merge the left and right
  // listeners, depth-ordered.
  if (inode.right && inode.right != inode.left) {
    // TODO Unnecessary copy
    listener_set right_listeners = m_listeners.GetListeners(inode.right);
    for (listener_iter i = right_listeners.begin();
         i != right_listeners.end(); ++i) {
      changes_by_depth[(*i)->depth].push_back(change_pair(*i, NULL));
    }
  }

  if (changes_by_depth.empty()) {
    m_log.info("Connector: No listeners of " + string(inode) + " (left: " + string(*inode.left) + "; right: " + (inode.right ? string(*inode.right) : "<null>") + " to update.");
  }

  while (!changes_by_depth.empty()) {
    // Update the deepest depth of our changeset
    int depth = changes_by_depth.rbegin()->first;
    change_vec changes = changes_by_depth.rbegin()->second;
    m_log.debug("Connector: Updating changes at depth " + lexical_cast<string>(depth));
    for (change_iter i = changes.begin(); i != changes.end(); ++i) {
      bool changed = UpdateNode(*i->first, i->second);
      if (changed) {
        TreeDS* parent = i->first->parent;
        if (parent) {
          changes_by_depth[parent->depth].push_back(make_pair(parent, i->first));
        }
      }
    }
    changes_by_depth.erase(depth);
  }
}

void Connector::DrawGraph(const TreeDS& onode, const IList* inode) {
  if (!m_istart) {
    return;
  }
  m_grapher->AddIList(m_name, *m_istart, m_name + " input");
  m_grapher->AddOTree(m_name, m_root, m_name + " output");
  m_grapher->AddIListeners(m_name, *this, *m_istart);
  if (inode) {
    m_grapher->Signal(m_name, &onode);
    m_grapher->Signal(m_name, inode);
  } else {
    m_grapher->Signal(m_name, &onode, true);
  }
  m_grapher->SaveAndClear();
}
