// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Connector_h_
#define _Connector_h_

#include "DS.h"
#include "ListenerTable.h"
#include "Rule.h"
#include "TreeChangeset.h"

#include "util/Log.h"

#include <map>
#include <set>

namespace fw {

template <typename INode>
class Connector {
public:
  Connector(Log& log, const Rule& rule)
    : m_log(log),
      m_root(rule.MakeState(), NULL) {}

  // These return the common ancestor of all nodes that were changed, if any.
  // Insert() a new inode, AFTER attaching its connections in the input DS.
  void Insert(const INode& inode);
  // Delete() an inode.  Call this AFTER updating its left and right to point
  // to each other, but leave this inode's left and right pointers intact.
  void Delete(const INode& inode);

  // These wrap Insert()s, Delete()s, and UpdateListeners()s, for Tree-Tree only
  void Change(const TreeChange& change) {
    switch (change.type) {
    case TreeChange::INSERT:
      Insert(*change.node);
      break;
    case TreeChange::UPDATE:
      UpdateListeners(*change.node);
      break;
    case TreeChange::DELETE:
      Delete(*change.node);
      break;
    default:
      throw FWError("Connector::Update: Unsupported Update type");
    }
  }
  void Change(const TreeChangeset::changeset_map& changeset) {
    for (TreeChangeset::changeset_rev_iter i = changeset.rbegin();
         i != changeset.rend(); ++i) {
      for (TreeChangeset::change_iter j = i->second.begin();
           j != i->second.end(); ++j) {
        Change(*j);
      }
    }
  }

  // Insert() and Delete() will track a changeset of tree updates that have
  // occurred.  Use these to get or clear the changeset.
  const TreeChangeset::changeset_map& GetChangeset() const { return m_changeset.GetChangeset(); }
  void ClearChangeset() { m_changeset.Clear(); }

  // Insert, Reposition, or Update a single node.  Either called internally by
  // the Connector or by a rule that wants to process a relative.
  // Insert does not wire up the TreeDS nodes, but it adds an "Insert"
  // operation to our changeset, and positions the node.
  void InsertNode(TreeDS& x, const INode& inode) {
    m_log.info("Connector: Inserting " + std::string(x) + " with inode " + std::string(inode));
    RuleState& state = x.GetState<RuleState>();
    m_changeset.AddTreeChange(TreeChange(TreeChange::INSERT, x));
    state.rule.Reposition(*this, x, inode);
  }

  // Set the "starting" inode.  The rule's Reposition() may RepositionNode() on
  // its children.
  void RepositionNode(TreeDS& x, const INode& inode) {
    m_log.info("Connector: Repositioning " + std::string(x) + " with inode " + std::string(inode));
    if (x.istart == &inode) {
      m_log.info(" - already at the right position; skipping");
      return;
    }
    RuleState& state = x.GetState<RuleState>();
    m_changeset.AddTreeChange(TreeChange(TreeChange::UPDATE, x));
    state.rule.Reposition(*this, x, inode);
  }

  // Recalculate state based on a change to a child.  Child could be NULL
  // meaning the update is called by a direct-subscription.
  bool UpdateNode(TreeDS& x, const TreeDS* child) {
    m_log.info("Connector: Updating " + std::string(x) + " with child " + (child ? std::string(*child) : "<null>"));
    RuleState& state = x.GetState<RuleState>();
    bool changed = state.rule.Update(*this, x, child);
    if (changed) {
      m_changeset.AddTreeChange(TreeChange(TreeChange::UPDATE, x));
    }
    return changed;
  }

  void ClearNode(TreeDS& x) {
    m_log.info("Connector: Clearing " + std::string(x));
    for (TreeDS::child_mod_iter i = x.children.begin();
         i != x.children.end(); ++i) {
      ClearNode(*i);
    }
    m_listeners.RemoveAllListenings(&x);
    x.Clear();
    m_changeset.AddTreeChange(TreeChange(TreeChange::DELETE, x));
  }

  // Called from a rule/state regarding its DS node.  Listens for updates to
  // this inode.
  void Listen(TreeDS& x, const INode& inode) {
    m_log.info("Connector: " + std::string(x) + " will listen to " + std::string(inode));
    m_listeners.AddListener(&inode, &x);
  }
  void Unlisten(TreeDS& x, const INode& inode) {
    m_log.info("Connector: " + std::string(x) + " will NOT listen to " + std::string(inode));
    m_listeners.RemoveListener(&inode, &x);
  }

private:
  typedef typename ListenerTable<const INode*, TreeDS*>::listener_set listener_set;
  typedef typename ListenerTable<const INode*, TreeDS*>::listener_iter listener_iter;

  // Convenience core for Insert() and Delete().  Updates all listeners to the
  // left (and if any and distinct, to the right) of the inode, about the
  // inode.  For TreeDS, updates all listeners of the parent of the inode.
  void UpdateListeners(const INode& inode);

  Log& m_log;
  TreeDS m_root;
  ListenerTable<const INode*, TreeDS*> m_listeners;
  TreeChangeset m_changeset;
};

template <>
void Connector<ListDS>::Insert(const ListDS& inode);
template <>
void Connector<TreeDS>::Insert(const TreeDS& inode);

template <>
void Connector<ListDS>::Delete(const ListDS& inode);
template <>
void Connector<TreeDS>::Delete(const TreeDS& inode);

template <>
void Connector<ListDS>::UpdateListeners(const ListDS& inode);
template <>
void Connector<TreeDS>::UpdateListeners(const TreeDS& inode);

}

#endif // _Connector_h_
