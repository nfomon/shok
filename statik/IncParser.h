// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_IncParser_h_
#define _statik_IncParser_h_

#include "Batch.h"
#include "List.h"
#include "ListenerTable.h"
#include "ObjectPool.h"
#include "OrderList.h"
#include "OutputFunc.h"
#include "ParseAction.h"
#include "Rule.h"
#include "STree.h"

#include <deque>
#include <map>
#include <memory>
#include <set>

namespace statik {

class Grapher;

class IncParser {
public:
  typedef typename ListenerTable<const List*, STree*>::listener_set listener_set;
  typedef typename ListenerTable<const List*, STree*>::listener_iter listener_iter;

  IncParser(std::auto_ptr<Rule> grammar,
            const std::string& name = "",
            const std::string& graphdir = "");

  const STree& GetRoot() const; // for tests

  // Insert a new item, after position pos (NULL for start).
  void Insert(const List& inode, const List* pos);
  // Delete an item.
  void Delete(const List& inode);
  // Update an item with a new value.
  void Update(const List& inode, const std::string& value);
  // Apply a batch of inode insertions/updates/deletions
  void ApplyBatch(const Batch& batch);

  void ExtractChanges(Batch& out_batch);

  void Enqueue(ParseAction action);

  // Called from a rule, regarding its parse node.
  // Listens for updates to this inode.
  void Listen(STree& x, const List& inode);
  void Unlisten(STree& x, const List& inode);

  // Called from a node that's being restarted, to cancel all its listening
  void UnlistenAll(STree& x);

  // IncParser owns the STree nodes.  This allows a ParseFunc() to "unlink" a
  // node from the tree before it is actually deleted, so that an OutputFunc()
  // can refer to its destruction safely.
  STree* OwnNode(std::auto_ptr<STree> node);

  // Prepare a node to be removed completely
  void ClearNode(STree& x);

  // Get the first node of the input list
  const List* GetFirstINode() const;

  // Get the first node of the output list
  const List* GetFirstONode() const;

  // Indicate that a node has been computed.  Called by STree::ParseNode()
  void TouchNode(const STree& node);

  int INodeCompare(const List& a, const List& b) const;

  listener_set GetListeners(const List& x) const;
  void DrawGraph(const STree& onode,
                 const List* inode = NULL,
                 const Batch* batch = NULL,
                 const STree* initiator = NULL);

  std::string Name() const { return m_name; }

private:
  typedef std::deque<ParseAction> action_queue;
  typedef action_queue::const_iterator action_iter;
  typedef action_queue::iterator action_mod_iter;
  typedef std::map<STree::depth_t, action_queue> action_map;
  typedef std::map<const STree*, OutputList> output_map;
  typedef output_map::const_iterator output_iter;
  typedef output_map::iterator output_mod_iter;
  typedef std::map<const List*, List*> input_map;
  typedef input_map::const_iterator input_iter;
  typedef input_map::iterator input_mod_iter;

  // Called by ApplyBatch() and public Insert()/Delete()/Update()
  void InsertNode(List& inode);
  void DeleteNode(List& inode);
  void UpdateNode(List& inode);
  void ProcessActions();

  void ComputeOutput_Update(const STree& node, Batch& out_batch, const List*& behind_node);
  void ComputeOutput_Insert(const STree& node, Batch& out_batch, const List*& behind_node);
  void ComputeOutput_Delete(const STree& node, Batch& out_batch);
  void Cleanup();

  // Convenience wrappers for ComputeOutput_*
  void InsertOutput(const OutputItem& item, Batch& out_batch, const List*& behind_node);
  void RemoveOutput(const OutputItem& item, Batch& out_batch);
  void UpdateOutput(OutputItem& item, Batch& out_batch, const List*& behind_node);
  const List* GetOEnd(const OutputItem& item) const;

  void SanityCheck();
  void SanityCheck(const STree* s) const;

  Rule m_rootRule; // Rule for the Root node
  STree m_root; // Root of the parse tree
  std::string m_name;
  std::auto_ptr<Grapher> m_grapher;
  ObjectPool<List> m_ilistPool;
  input_map m_inputMap;
  ObjectPool<STree> m_nodePool;
  OrderList m_orderList;
  action_map m_actions_by_depth;
  ListenerTable<const List*, STree*> m_listeners;

  output_map m_outputPerNode;
  std::set<const STree*> m_touchedNodes;
  List* m_firstINode;
  int m_sancount;
};

}

#endif // _statik_IncParser_h_
