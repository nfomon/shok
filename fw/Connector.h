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

  // Insert() a new inode, AFTER attaching its connections in the input DS.
  void Insert(const INode& inode);
  // Delete() an inode.  Call this AFTER updating its left and right to point
  // to each other, but leave this inode's left and right pointers intact.
  void Delete(const INode& inode);

  // Reposition or Update a single node.  Used internally by the Connector but
  // can be called by a rule for convenience sake as well.  The Connector
  // assures that the children will have already been updated if necessary.
  void RepositionNode(TreeDS& x, const INode& inode) {
    m_log.info("Connector: Repositioning " + std::string(x) + " with inode " + std::string(inode));
    RuleState& state = x.GetState<RuleState>();
    state.rule.Reposition(*this, x, inode);
  }
  bool UpdateNode(TreeDS& x, const INode& inode) {
    m_log.info("Connector: Updating " + std::string(x) + " with inode " + std::string(inode));
    RuleState& state = x.GetState<RuleState>();
    return state.rule.Update(*this, x, inode);
  }
  void ClearNode(TreeDS& x) {
    m_log.info("Connector: Clearing " + std::string(x));
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
  // left (and if any and distinct, to the right) of the inode, about the inode.
  void UpdateListeners(const INode& inode);

  Log& m_log;
  TreeDS m_root;
  ListenerTable<const INode*, TreeDS*> m_listeners;
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
