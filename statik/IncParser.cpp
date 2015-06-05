// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "IncParser.h"

#include "Batch.h"
#include "Grapher.h"
#include "List.h"
#include "Root.h"
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

IncParser::IncParser(auto_ptr<Rule> grammar,
                     const string& name,
                     const string& graphdir)
  : m_rootRule(name, MakeParseFunc_Root(name), MakeOutputFunc_Pass()),
    m_root(*this, m_rootRule, /*parent*/ NULL),
    m_name(name),
    m_firstINode(NULL),
    m_sancount(0) {
  m_rootRule.AddChild(grammar);
  if (!graphdir.empty()) {
    m_grapher.reset(new Grapher(graphdir, string(name + "_")));
    m_grapher->AddMachine(name, m_rootRule);
    m_grapher->SaveAndClear();
  }
}

const STree& IncParser::GetRoot() const {
  return m_root;
}

INode IncParser::Insert(const List& inode, INode pos) {
  auto_ptr<List> node(new List(inode));
  INode n = node.get();
  m_ilistPool.Insert(node);
  if (pos) {
    if (&inode == pos) {
      throw SError("Cannot Insert node " + string(inode) + " after itself");
    }
    n->left = pos;
    n->right = pos->right;
    if (n->right) {
      n->right->left = n;
    }
    pos->right = n;
  } else {
    List* first = m_firstINode;
    if (first) {
      n->right = first;
      first->left = n;
    }
    m_firstINode = n;
  }
  Batch b;
  b.Insert(*n);
  ApplyBatch(b);
  return n;
}

void IncParser::Delete(INode inode) {
  if (inode->left) {
    inode->left->right = inode->right;
  }
  if (inode->right) {
    inode->right->left = inode->left;
  }
  if (m_firstINode == inode) {
    m_firstINode = inode->right;
  }
  m_ilistPool.Unlink(*inode);
  Batch b;
  b.Delete(*inode);
  ApplyBatch(b);
}

void IncParser::Update(INode inode, const string& value) {
  inode->value = value;
  Batch b;
  b.Update(*inode);
  ApplyBatch(b);
}

void IncParser::ApplyBatch(const Batch& batch) {
  g_log.info() << "Updating IncParser " << m_name << " with batch of size " << batch.Size();
  for (Batch::batch_iter i = batch.begin(); i != batch.end(); ++i) {
    switch (i->second) {
    case Batch::OP_INSERT:
      InsertNode(*i->first);
      break;
    case Batch::OP_DELETE:
      DeleteNode(*i->first);
      break;
    case Batch::OP_UPDATE:
      UpdateNode(*i->first);
      break;
    default:
      throw SError("Cannot update with batch with unknown operation");
    }
  }
  g_log.info() << "Done updating IncParser " << m_name << " with batch of size " << batch.Size();
}

void IncParser::ExtractChanges(Batch& out_batch) {
  g_log.debug() << "EXTRACT START: " << m_name;
  if (m_touchedNodes.find(&m_root) != m_touchedNodes.end()) {
    ComputeOutput_Update(m_root, out_batch);
  }
  g_log.debug() << "EXTRACT DONE: " << m_name;
  m_touchedNodes.clear();
  Cleanup();
  if (!out_batch.IsEmpty()) {
    DrawGraph(m_root, /*inode*/ NULL, &out_batch);
  }
}

void IncParser::Enqueue(ParseAction action) {
  g_log.info() << "IncParser " << m_name << ": Enqueuing action " << ParseAction::UnMapAction(action.action) << " - " << *action.node << " at depth " << action.node->GetDepth();
  int depth = action.node->GetDepth();
  action_map::iterator ai = m_actions_by_depth.find(depth);
  if (m_actions_by_depth.end() == ai) {
    action_queue actions;
    actions.push_back(action);
    m_actions_by_depth.insert(std::make_pair(depth, actions));
  } else {
    // TODO if an equivalent action is already enqueued, just drop this one
    ai->second.push_back(action);
  }
}

void IncParser::Listen(STree& x, const List& inode) {
  if (m_listeners.IsListening(&inode, &x)) {
    g_log.info() << "IncParser: " << x << " is already listening to " << inode;
  } else {
    g_log.info() << "IncParser: " << x << " will listen to " << inode;
    m_listeners.AddListener(&inode, &x);
  }
}

void IncParser::Unlisten(STree& x, const List& inode) {
  g_log.info() << "IncParser: " << x << " will NOT listen to " << inode;
  m_listeners.RemoveListener(&inode, &x);
}

void IncParser::UnlistenAll(STree& x) {
  g_log.info() << "IncParser " << m_name << ": Unlistening all from node " << x;
  m_listeners.RemoveAllListenings(&x);
}

STree* IncParser::OwnNode(auto_ptr<STree> node) {
  g_log.debug() << "IncParser " << m_name << ": Owning node " << string(*node);
  return m_nodePool.Insert(node);
}

void IncParser::ClearNode(STree& x) {
  m_listeners.RemoveAllListenings(&x);
  if (&x != &m_root) {
    g_san.debug() << "xx Unlinking node " << x << " - " << &x;
    m_nodePool.Unlink(x);
  }
}

const List* IncParser::GetFirstINode() const {
  return m_firstINode;
}

const List* IncParser::GetFirstONode() const {
  return m_root.GetOutputFunc().OStart();
}

void IncParser::TouchNode(const STree& node) {
  m_touchedNodes.insert(&node);
}

int IncParser::INodeCompare(const List& a, const List& b) const {
  return m_orderList.Compare(a, b);
}

IncParser::listener_set IncParser::GetListeners(const List& x) const {
  return m_listeners.GetListeners(&x);
}

void IncParser::DrawGraph(const STree& onode, const List* inode, const Batch* batch, const STree* initiator) {
  if (!m_grapher.get()) {
    return;
  }
  SanityCheck();
  const List* istart = GetFirstINode();
  if (istart) {
    m_grapher->AddIList(m_name, *istart, m_name + " input");
  }
  m_grapher->AddSTree(m_name, m_root, m_name + " output", initiator);
  const List* ostart = GetFirstONode();
  if (ostart) {
    m_grapher->AddOList(m_name, *ostart, m_name + " olist");
  }
  if (batch) {
    g_log.debug() << "Drawing batch: " << batch->Print();
    m_grapher->AddBatch(m_name, *batch);
  }
  if (istart) {
    m_grapher->AddIListeners(m_name, *this, *istart);
  }
  if (inode) {
    m_grapher->Signal(m_name, &onode);
    m_grapher->Signal(m_name, inode);
  } else {
    m_grapher->Signal(m_name, &onode, true);
  }
  m_grapher->SaveAndClear();
}

/* private */

void IncParser::InsertNode(const List& inode) {
  g_log.info() << "IncParser " << m_name << ": Inserting INode: " << inode << " with " << (inode.left ? "left" : "no left") << " and " << (inode.right ? "with a right" : "no right");
  m_orderList.Insert(inode);
  if (m_grapher.get()) {
    if (GetFirstINode()) {
      m_grapher->AddOrderList(m_name, m_orderList, *GetFirstINode());
    } else {
      m_grapher->AddOrderList(m_name, m_orderList, inode);
    }
    m_grapher->SaveAndClear();
  }

  if (m_root.IsClear()) {
    g_log.debug() << " - Clear root; restarting the tree root";
    Enqueue(ParseAction(ParseAction::Start, m_root, inode));
  } else {
    if (inode.left) {
      g_log.debug() << " - Found left inode " << *inode.left << ": updating listeners of " << inode << "'s left and (if present) right";
      listener_set listeners = m_listeners.GetListeners(inode.left);
      for (listener_iter i = listeners.begin(); i != listeners.end(); ++i) {
        Enqueue(ParseAction(ParseAction::INodeInsert, **i, inode));
      }
      if (inode.right) {
        listeners = m_listeners.GetListeners(inode.right);
        for (listener_iter i = listeners.begin(); i != listeners.end(); ++i) {
          Enqueue(ParseAction(ParseAction::INodeInsert, **i, inode));
        }
      }
    } else {
      g_log.debug() << " - No left inode: prepending behind the root";
      Enqueue(ParseAction(ParseAction::Restart, m_root, inode));
    }
  }

  ProcessActions();

  // Assert that the inode has at least one listener in the updated set.
  if (!m_listeners.HasAnyListeners(&inode)) {
    m_root.GetState().GoBad();
  }
  // Stronger check: every inode has at least one listener :)
  g_log.debug() << "IncParser: Insert done";
}

void IncParser::DeleteNode(const List& inode) {
  g_log.info() << "Deleting list inode " << inode;
  if (m_grapher.get()) {
    if (GetFirstINode()) {
      m_grapher->AddOrderList(m_name, m_orderList, *GetFirstINode());
    } else {
      m_grapher->AddOrderList(m_name, m_orderList, inode);
    }
    m_grapher->SaveAndClear();
  }

  if (!inode.left && !inode.right) {
    g_log.debug() << "IncParser: Deleted node has no left or right; clearing root";
    m_root.ClearNode(inode);
  } else {
    if (inode.left) {
      g_log.debug() << "IncParser: Deleted node has inode.left, so enqueuing INodeDelete actions on its listeners";
      listener_set listeners = m_listeners.GetListeners(inode.left);
      for (listener_iter i = listeners.begin(); i != listeners.end(); ++i) {
        Enqueue(ParseAction(ParseAction::INodeDelete, **i, inode));
      }
    }
    listener_set listeners = m_listeners.GetListeners(&inode);
    g_log.debug() << "IncParser: Deleted node has listeners, so either clearing them or sending INodeDelete";
    for (listener_iter i = listeners.begin(); i != listeners.end(); ++i) {
      if (&(*i)->IStart() == &inode) {
        (*i)->ClearNode(inode);
      } else {
        Enqueue(ParseAction(ParseAction::INodeDelete, **i, inode));
      }
    }
  }

  ProcessActions();
  m_listeners.RemoveAllListeners(&inode);
  m_orderList.Delete(inode);
  g_log.debug() << "IncParser: Delete done";
}

void IncParser::UpdateNode(const List& inode) {
  g_log.info() << "IncParser " << m_name << ": Updating listeners of inode " << inode;
  listener_set listeners = m_listeners.GetListeners(&inode);
  for (listener_iter i = listeners.begin(); i != listeners.end(); ++i) {
    Enqueue(ParseAction(ParseAction::INodeUpdate, **i, inode));
  }
}

void IncParser::ProcessActions() {
  while (!m_actions_by_depth.empty()) {
    STree::depth_t depth = m_actions_by_depth.rbegin()->first;
    action_queue& actions = m_actions_by_depth.rbegin()->second;
    g_log.debug() << "IncParser " << m_name << ": Applying actions at depth " << depth;
    while (m_actions_by_depth.rbegin()->first == depth && !actions.empty()) {
      ParseAction* a = &actions.front();
      actions.pop_front();
      if (ParseAction::Start == a->action) {
        if (!a->node->IsClear()) {
          throw SError("Why are we trying to Start a node that isn't clear?");
        }
        a->node->StartNode(*a->inode);
      } else {
        a->node->ParseNode(a->action, *a->inode, a->initiator);
      }
    }
    if (m_actions_by_depth.at(depth).empty()) {
      g_log.debug() << "IncParser " << m_name << " Clearing actions at depth " << depth;
      m_actions_by_depth.erase(depth);
    }
  }
}

void IncParser::ComputeOutput_Update(const STree& node, Batch& out_batch) {
  g_log.debug() << "IncParser " << m_name << ": Computing output UPDATE for node " << node;

  node.GetOutputFunc() ();
  const OutputList& output = node.GetOutputFunc().GetOutput();
  output_mod_iter prev_output_i = m_outputPerNode.find(&node);

  if (m_outputPerNode.end() == prev_output_i) {
    // We weren't emitting before, but are emitting now.  Insert all.
    g_log.debug() << " - was not emitting, but is now; inserting all";
    return ComputeOutput_Insert(node, out_batch);
  }

  OutputList& prev_output = prev_output_i->second;
  g_log.debug() << node << " was emitting " << prev_output.size() << " items";
  g_log.debug() << node << " is now emitting " << output.size() << " items";

  // Linear pass through both lists, comparing elements.
  OutputList::const_iterator item = output.begin();
  OutputList::const_iterator prev_item = prev_output.begin();
  while (item != output.end() && prev_item != prev_output.end()) {
    if (output.end() == item) {
      RemoveOutput(*prev_item, out_batch);
      ++prev_item;
    } else if (prev_output.end() == prev_item) {
      InsertOutput(*item, out_batch);
      ++item;
    } else if (&*item == &*prev_item) {
      UpdateOutput(*item, out_batch);
      ++item;
      ++prev_item;
    } else {
      // Buffer both sides, and keep a lookup-set so we can identify if/when we
      // find a node in one list that was surpassed in the other.
      typedef set<const OutputItem*> node_set;
      typedef node_set::const_iterator node_mod_iter;
      node_set ins_set;
      node_set del_set;
      OutputList::const_iterator ins_item = item;
      OutputList::const_iterator del_item = prev_item;
      while (ins_item != output.end() && del_item != prev_output.end()) {
        node_mod_iter find_ins = ins_set.find(&*prev_item);
        if (find_ins != ins_set.end()) {
          // insert nodes up to find_ins
          do {
            InsertOutput(*item, out_batch);
            ++item;
          } while (*find_ins != &*item);
          // then remove the nodes up to find_ins from ins_set, and advance item to find_ins+1
            // - actually, we can just nix the ins_set, since we'll break. meh
          ++item;
          // prev_item stays where it is
          // then break
          break;
        }
        node_mod_iter find_del = del_set.find(&*item);
        if (find_del != del_set.end()) {
          do {
            RemoveOutput(*prev_item, out_batch);
            ++prev_item;
          } while (*find_del != &*prev_item);
          ++prev_item;
          break;
        }
        ins_set.insert(&*ins_item);
        del_set.insert(&*del_item);
        ++ins_item;
        ++del_item;
      }
    }
  }
  for (; item != output.end(); ++item) {
    InsertOutput(*item, out_batch);
  }
  for (; prev_item != prev_output.end(); ++prev_item) {
    RemoveOutput(*prev_item, out_batch);
  }

  if (output.empty()) {
    m_outputPerNode.erase(&node);
  } else {
    m_outputPerNode[&node] = output;  // copy
  }

  g_log.debug() << "Done computing UPDATE output for node " << node << ".  Batch so far: " << out_batch.Print();
  node.GetOutputFunc().Sync();
}

void IncParser::ComputeOutput_Insert(const STree& node, Batch& out_batch) {
  g_log.debug() << "IncParser " << m_name << ": Computing output INSERT for node " << node;

  node.GetOutputFunc() ();
  const OutputList& output = node.GetOutputFunc().GetOutput();

  // Just insert everything the node is currently emitting; disregard its
  // previous output
  for (OutputList::const_iterator item = output.begin();
       item != output.end(); ++item) {
    InsertOutput(*item, out_batch);
  }

  if (output.empty()) {
    m_outputPerNode.erase(&node);
  } else {
    m_outputPerNode[&node] = output;  // copy
  }
  g_log.debug() << "Done computing INSERT output for node " << node << ".  Batch so far: " << out_batch.Print();
  node.GetOutputFunc().Sync();
}

void IncParser::ComputeOutput_Delete(const STree& node, Batch& out_batch) {
  g_log.debug() << "IncParser " << m_name << ": Computing output DELETE for node " << node;

  output_mod_iter prev_output_i = m_outputPerNode.find(&node);
  if (m_outputPerNode.end() == prev_output_i) {
    return;
  }
  OutputList& prev_output = prev_output_i->second;

  // Just delete everything the node was previously emitting; don't compute its
  // current output
  for (OutputList::const_iterator prev_item = prev_output.begin();
       prev_item != prev_output.end(); ++prev_item) {
    RemoveOutput(*prev_item, out_batch);
  }
  m_outputPerNode.erase(&node);
  g_log.debug() << "Done computing DELETE output for node " << node << ".  Batch so far: " << out_batch.Print();
}

void IncParser::Cleanup() {
  g_log.debug() << "IncParser " << m_name << " - cleaning up";
  g_san.debug() << "Cleaning up node pool: " << string(m_nodePool);
  m_nodePool.Cleanup();
  g_san.debug() << "Cleaning up IList pool: " << string(m_ilistPool);
  m_ilistPool.Cleanup();
  SanityCheck();
}

void IncParser::InsertOutput(const OutputItem& item, Batch& out_batch) {
  if (item.onode) {
    out_batch.Insert(*item.onode);
  } else {
    ComputeOutput_Insert(*item.child, out_batch);
  }
}

void IncParser::RemoveOutput(const OutputItem& item, Batch& out_batch) {
  if (item.onode) {
    out_batch.Delete(*item.onode);
  } else {
    ComputeOutput_Delete(*item.child, out_batch);
  }
}

void IncParser::UpdateOutput(const OutputItem& item, Batch& out_batch) {
  if (item.onode) {
    out_batch.Update(*item.onode);
  } else {
    ComputeOutput_Update(*item.child, out_batch);
  }
}

void IncParser::SanityCheck() {
  g_log.debug() << "Sanity check " << m_sancount;
  g_san.info();
  g_san.info() << "Sanity check " << m_sancount;
  SanityCheck(&m_root);
  ++m_sancount;
}

void IncParser::SanityCheck(const STree* s) const {
  g_san.info() << "SNode: " << s;
  g_san.info() << ":: " << *s;
  if (!s->GetIConnection().IsClear()) {
    const List* inode = &s->IStart();
    while (inode) {
      g_san.info() << " - INode: " << inode;
      g_san.info() << " - :: " << *inode;
      if (inode == &s->IEnd()) {
        break;
      }
      inode = inode->right;
    }
  }
  for (STree::child_iter child = s->children.begin(); child != s->children.end(); ++child) {
    g_san.info() << " - child: " << *child;
    g_san.info() << " - :: " << **child;
  }
  for (STree::child_iter child = s->children.begin(); child != s->children.end(); ++child) {
    if ((*child)->GetParent() != s) {
      g_san.info() << " - child " << *child << " parent check fail; child's parent is: " << (*child)->GetParent();
    }
  }
  for (STree::child_iter child = s->children.begin(); child != s->children.end(); ++child) {
    g_san.info() << s << " (" << *s << ") -> " << *child;
    SanityCheck(*child);
  }
  const List* onode = s->GetOutputFunc().OStart();
  while (onode) {
    g_san.info() << " - ONode: " << onode;
    g_san.info() << " - :: " << *onode;
    if (onode == s->GetOutputFunc().OEnd()) {
      break;
    }
    onode = onode->right;
  }
  g_san.info() << "Sanity done for " << s;
}
