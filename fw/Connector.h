// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Connector_h_
#define _Connector_h_

#include "DS.h"
#include "ListenerTable.h"
#include "Rule.h"

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

  const Hotlist& GetHotlist() const { return m_hotlist; }
  void ClearHotlist() { m_hotlist.clear(); }

  // These return the common ancestor of all nodes that were changed, if any.
  // Insert() a new inode, AFTER attaching its connections in the input DS.
  void Insert(const INode& inode);
  // Delete() an inode.  Call this AFTER updating its left and right to point
  // to each other, but leave this inode's left and right pointers intact.
  void Delete(const INode& inode);

  void UpdateWithHotlist(const Hotlist& hotlist) {
    for (Hotlist_iter i = hotlist.begin(); i != hotlist.end(); ++i) {
      State& state = (*i)->GetState();
      if (state.done) {
        Insert(**i);
      } else {
        Delete(**i);
      }
    }
  }

  // Reposition or Update a single node.  Either called internally by the
  // Connector or by a rule that wants to process a relative.

  // Set the "starting" inode.  The rule's Reposition() may RepositionNode() on
  // its children.
  void RepositionNode(TreeDS& x, const INode& inode) {
    m_log.info("Connector: Repositioning " + std::string(x) + " with inode " + std::string(inode));
    if (x.istart == &inode) {
      m_log.info(" - already at the right position; skipping");
      return;
    }
    RuleState& state = x.GetState<RuleState>();
    state.rule.Reposition(*this, x, inode);
  }

  // Recalculate state based on a change to a child.  Child could be NULL
  // meaning the update is called by a direct-subscription.
  bool UpdateNode(TreeDS& x, const TreeDS* child) {
    m_log.info("Connector: Updating " + std::string(x) + " with child " + (child ? std::string(*child) : "<null>"));
    RuleState& state = x.GetState<RuleState>();
    return state.rule.Update(*this, x, child);
  }

  void ClearNode(TreeDS& x) {
    m_log.info("Connector: Clearing " + std::string(x));
    for (TreeDS::child_mod_iter i = x.children.begin();
         i != x.children.end(); ++i) {
      ClearNode(*i);
    }
    m_listeners.RemoveAllListenings(&x);
    x.Clear();
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
  // Root of the output tree.  Tells us the root of the rule tree.
  TreeDS m_root;
  ListenerTable<const INode*, TreeDS*> m_listeners;
  Hotlist m_hotlist;
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
