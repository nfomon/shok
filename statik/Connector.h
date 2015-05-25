// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Connector_h_
#define _Connector_h_

#include "ConnectorAction.h"
#include "Hotlist.h"
#include "IList.h"
#include "ListenerTable.h"
#include "ObjectPool.h"
#include "OrderList.h"
#include "OutputFunc.h"
#include "Rule.h"
#include "STree.h"

#include <deque>
#include <map>
#include <set>

namespace statik {

class Grapher;

class Connector {
public:
  typedef typename ListenerTable<const IList*, STree*>::listener_set listener_set;
  typedef typename ListenerTable<const IList*, STree*>::listener_iter listener_iter;

  Connector(Rule& rule, const std::string& name = "", const std::string& graphdir = "");

  const STree& GetRoot() const; // for tests
  const State& GetState() const;

  void ExtractHotlist(Hotlist& out_hotlist);
  listener_set GetListeners(const IList& x) const;

  // Insert() a new inode.  Call this AFTER attaching its connections in the
  // input list.
  void Insert(const IList& inode);
  // Delete() an inode.  Call this AFTER updating its left and right to point
  // to each other, but leave this inode's left and right pointers intact.
  void Delete(const IList& inode);
  // Update() the listeners of an inode, to let them know it has updated.
  void Update(const IList& inode);

  // Apply a bunch of inode insertions/updates/deletions
  void UpdateWithHotlist(const Hotlist::hotlist_vec& hotlist);

  void Enqueue(ConnectorAction action);

  // Called from a rule/state regarding its DS node.
  // Listens for updates to this inode.
  void Listen(STree& x, const IList& inode);
  void Unlisten(STree& x, const IList& inode);

  // Called from a node that's being restarted, to cancel all its listening
  void UnlistenAll(STree& x);

  // Connector owns the STree nodes.  This allows a ComputeFunc() to "unlink" a
  // node from the tree before it is actually deleted, so that an OutputFunc()
  // can refer to its destruction safely.
  STree* OwnNode(std::auto_ptr<STree> node);

  // Prepare a node to be removed completely
  void ClearNode(STree& x);

  // Get the first node of the input list
  const IList* GetFirstINode() const;

  // Get the first node of the output list
  const IList* GetFirstONode() const;

  // Indicate that a node has been computed.  Called by STree::ComputeNode()
  void TouchNode(const STree& node);

  int INodeCompare(const IList& a, const IList& b) const;

  void DrawGraph(const STree& onode,
                 const IList* inode = NULL,
                 const Hotlist* hotlist = NULL,
                 const STree* initiator = NULL);

  void SanityCheck();
  void SanityCheck(const STree* s) const;

  std::string Name() const { return m_name; }

private:
  typedef std::deque<ConnectorAction> action_queue;
  typedef action_queue::const_iterator action_iter;
  typedef action_queue::iterator action_mod_iter;
  typedef std::map<STree::depth_t, action_queue> action_map;

  // Called by UpdateWithHotlist()
  void InsertNode(const IList& inode);
  void DeleteNode(const IList& inode);
  void UpdateNode(const IList& inode);

  void ProcessActions();
  void ComputeOutput_Update(const STree& node, Hotlist& out_hotlist);
  void ComputeOutput_Insert(const STree& node, Hotlist& out_hotlist);
  void ComputeOutput_Delete(const STree& node, Hotlist& out_hotlist);
  void CleanupIfNeeded();

  Rule m_rootRule; // Rule for the Root node
  STree m_root; // Root of the output tree
  Rule& m_grammar; // Root of the Grammar
  std::string m_name;
  bool m_needsCleanup;
  std::auto_ptr<Grapher> m_grapher;
  ObjectPool<STree> m_nodePool;
  OrderList m_orderList;
  action_map m_actions_by_depth;
  ListenerTable<const IList*, STree*> m_listeners;
  typedef std::map<const STree*, OutputState> output_map;
  typedef output_map::const_iterator output_iter;
  typedef output_map::iterator output_mod_iter;
  output_map m_outputPerNode;
  std::set<const STree*> m_touchedNodes;
  int m_sancount;
};

}

#endif // _Connector_h_
