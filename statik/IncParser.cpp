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

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

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
  : m_rootRule(name, MakeParseFunc_Root(name), MakeOutputFunc_Root()),
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

void IncParser::Insert(const List& inode, const List* pos) {
  auto_ptr<List> node(new List(inode.name, inode.value));
  List* n = node.get();
  m_ilistPool.Insert(node);
  std::pair<input_iter, bool> p = m_inputMap.insert(std::make_pair(&inode, n));
  if (!p.second) {
    throw SError("Cannot double-insert input node " + inode.Print());
  }
  List* ipos = NULL;
  input_iter ipos_i = m_inputMap.find(pos);
  if (ipos_i != m_inputMap.end()) {
    ipos = ipos_i->second;
  }
  if (ipos) {
    if (n == ipos) {
      throw SError("Cannot Insert node " + inode.Print() + " after itself");
    }
    n->left = ipos;
    n->right = ipos->right;
    if (n->right) {
      n->right->left = n;
    }
    ipos->right = n;
    g_log.debug() << "INSERT::::::  for pos " << *pos << " we have ipos " << *ipos << " which has right " << *ipos->right;
  } else {
    List* first = m_firstINode;
    if (first) {
      first->left = n;
      n->right = first;
    }
    m_firstINode = n;
  }
  InsertNode(*n);
}

void IncParser::Delete(const List& inode) {
  input_mod_iter n = m_inputMap.find(&inode);
  if (m_inputMap.end() == n) {
    throw SError("Cannot delete input node " + inode.Print() + ": node not found");
  }
  List& node = *n->second;
  if (node.left) {
    node.left->right = node.right;
  }
  if (node.right) {
    node.right->left = node.left;
  }
  if (m_firstINode == &node) {
    m_firstINode = node.right;
  }
  m_ilistPool.Unlink(node);
  DeleteNode(node);
  m_inputMap.erase(n);
}

void IncParser::Update(const List& inode, const string& value) {
  input_iter n = m_inputMap.find(&inode);
  if (m_inputMap.end() == n) {
    throw SError("Cannot update input node " + inode.Print() + ": node not found");
  }
  List& node = *n->second;
  node.value = value;
  UpdateNode(node);
}

void IncParser::ApplyBatch(const Batch& batch) {
  g_log.info() << "Updating IncParser " << m_name << " with batch of size " << batch.Size();
  for (Batch::batch_iter i = batch.begin(); i != batch.end(); ++i) {
    switch (i->op) {
    case Batch::OP_INSERT:
      Insert(*i->node, i->pos);
      break;
    case Batch::OP_DELETE:
      Delete(*i->node);
      break;
    case Batch::OP_UPDATE:
      Update(*i->node, i->node->value);
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
    const List* behind_node = NULL;
    ComputeOutput_Update(m_root, out_batch, behind_node, State::ST_COMPLETE);
  }
  g_log.debug() << "EXTRACT DONE: " << m_name;
  m_touchedNodes.clear();
  for (std::vector<STree*>::iterator i = m_forcedChanges.begin(); i != m_forcedChanges.end(); ++i) {
    (*i)->UnforceChange();
  }
  m_forcedChanges.clear();
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
  g_log.debug() << "IncParser " << m_name << ": Owning node " << *node;
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

void IncParser::ForceChange(STree& node) {
  node.ForceChange();
  m_forcedChanges.push_back(&node);
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
  SanityCheck("DrawGraph");
  const List* istart = GetFirstINode();
  if (istart) {
    m_grapher->AddIList(m_name, *istart, m_name + " input");
  }
  m_grapher->AddSTree(m_name, m_root, m_name + " parse", initiator);
  if (batch) {
    m_grapher->AddOBatch(m_name, *batch, m_name + " output");
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

void IncParser::InsertNode(List& inode) {
  g_log.info() << "IncParser " << m_name << ": Inserting INode: " << inode << " with " << (inode.left ? "left" : "no left") << " and " << (inode.right ? "with a right" : "no right");
  m_orderList.Insert(inode);
  if (!m_firstINode) {
    m_firstINode = &inode;
  }
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

  // The inode should have at least one listener in the updated set.  If this
  // is violated, it's probably stuff past the end of accepted input.
  if (!m_listeners.HasAnyListeners(&inode)) {
    m_root.GetState().GoBad();
  }
  // Stronger check: every inode has at least one listener :)
  g_log.debug() << "IncParser: Insert done";
}

void IncParser::DeleteNode(List& inode) {
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
    if (listeners.empty()) {
      g_log.debug() << "IncParser: Deleted node has no listeners; taking no action";
    } else {
      g_log.debug() << "IncParser: Deleted node has listeners, so either clearing them or sending INodeDelete";
      for (listener_iter i = listeners.begin(); i != listeners.end(); ++i) {
        if (&(*i)->IStart() == &inode) {
          (*i)->ClearNode(inode);
        } else {
          Enqueue(ParseAction(ParseAction::INodeDelete, **i, inode));
        }
      }
    }
  }

  ProcessActions();
  m_listeners.RemoveAllListeners(&inode);
  m_deleteFromOrderListSoon.push_back(&inode);
  g_log.debug() << "IncParser: Delete done";
}

void IncParser::UpdateNode(List& inode) {
  g_log.info() << "IncParser " << m_name << ": Updating listeners of inode " << inode;
  listener_set listeners = m_listeners.GetListeners(&inode);
  for (listener_iter i = listeners.begin(); i != listeners.end(); ++i) {
    Enqueue(ParseAction(ParseAction::INodeUpdate, **i, inode));
  }
  ProcessActions();
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

void IncParser::ComputeOutput_Update(const STree& node, Batch& out_batch, const List*& behind_node, State::Station worst_station) {
  g_log.debug() << "IncParser " << m_name << ": Computing output UPDATE for node " << node << " with behind node " << behind_node;
  worst_station = std::min(node.GetState().GetStation(), worst_station);

  node.GetOutputFunc() ();
  OutputList& output = node.GetOutputFunc().GetOutput();
  output_mod_iter prev_output_i = m_outputPerNode.find(&node);

  if (m_outputPerNode.end() == prev_output_i) {
    // We weren't emitting before, but are emitting now.  Insert all.
    if (output.empty()) {
      g_log.debug() << " - was not emitting, but is now, but it has no items; done";
      return;
    }
    g_log.debug() << " - was not emitting, but is now; inserting all";
    return ComputeOutput_Insert(node, out_batch, behind_node, worst_station);
  }

  const OutputList& prev_output = prev_output_i->second;
  g_log.debug() << node << " was emitting " << prev_output.size() << " items";
  g_log.debug() << node << " is now emitting " << output.size() << " items";

  typedef set<OutputItem> node_set;
  typedef node_set::iterator node_mod_iter;
  node_set nodes(output.begin(), output.end());
  for (OutputList::const_iterator prev_item = prev_output.begin();
       prev_item != prev_output.end(); ++prev_item) {
    node_mod_iter find_del = nodes.find(*prev_item);
    if (nodes.end() == find_del) {
      //g_log.debug() << "Removing prev_item " << *prev_item; // UNSAFE
      RemoveOutput(*prev_item, out_batch, worst_station);
    } else {
      nodes.erase(find_del);
    }
  }
  for (OutputList::iterator item = output.begin();
       item != output.end(); ++item) {
    node_mod_iter find_ins = nodes.find(*item);
    if (nodes.end() == find_ins) {
      g_log.debug() << "Updating item " << *item << " after behind-node " << behind_node;
      UpdateOutput(*item, out_batch, behind_node, worst_station);
    } else {
      nodes.erase(find_ins);
      g_log.debug() << "Inserting item " << *item << " after behind-node " << behind_node;
      InsertOutput(*item, out_batch, behind_node, worst_station);
    }
    const List* b = GetOEnd(*item);
    if (b) {
      g_log.debug() << "Update of " << node << ": set behind_node to " << *b;
      behind_node = b;
    }
  }

  if (output.empty()) {
    m_outputPerNode.erase(&node);
  } else {
    m_outputPerNode[&node] = output;  // copy
  }

  g_log.debug() << "Done computing UPDATE output for node " << node << ".  Batch so far: " << out_batch;
  node.GetOutputFunc().Sync();
}

void IncParser::ComputeOutput_Insert(const STree& node, Batch& out_batch, const List*& behind_node, State::Station worst_station) {
  g_log.debug() << "IncParser " << m_name << ": Computing output INSERT for node " << node;
  worst_station = std::min(node.GetState().GetStation(), worst_station);
  if (State::ST_PENDING == worst_station || State::ST_BAD == worst_station) {
    g_log.debug() << "NOT computing output insert to node " << node << " since worst station is " << worst_station;
    return;
  }

  node.GetOutputFunc() ();
  const OutputList& output = node.GetOutputFunc().GetOutput();

  // Just insert everything the node is currently emitting; disregard its
  // previous output
  for (OutputList::const_iterator item = output.begin();
       item != output.end(); ++item) {
    InsertOutput(*item, out_batch, behind_node, worst_station);
    const List* b = GetOEnd(*item);
    if (b) {
      g_log.debug() << "Inserting " << node << "; set behind_node to " << *b;
      behind_node = b;
    }
  }

  if (output.empty()) {
    m_outputPerNode.erase(&node);
  } else {
    m_outputPerNode[&node] = output;  // copy
  }
  g_log.debug() << "Done computing INSERT output for node " << node << ".  Batch so far: " << out_batch;
  node.GetOutputFunc().Sync();
}

void IncParser::ComputeOutput_Delete(const STree& node, Batch& out_batch, State::Station worst_station) {
  //g_log.debug() << "IncParser " << m_name << ": Computing output DELETE for node " << node; // UNSAFE
  worst_station = std::min(node.GetState().GetStation(), worst_station);

  output_mod_iter prev_output_i = m_outputPerNode.find(&node);
  if (m_outputPerNode.end() == prev_output_i) {
    return;
  }
  OutputList& prev_output = prev_output_i->second;

  // Just delete everything the node was previously emitting; don't compute its
  // current output
  for (OutputList::const_iterator prev_item = prev_output.begin();
       prev_item != prev_output.end(); ++prev_item) {
    RemoveOutput(*prev_item, out_batch, worst_station);
  }
  m_outputPerNode.erase(&node);
  //g_log.debug() << "Done computing DELETE output for node " << node << ".  Batch so far: " << out_batch;  // UNSAFE
}

void IncParser::Cleanup() {
  g_log.debug() << "IncParser " << m_name << " - cleaning up";
  g_san.debug() << "Cleaning up order list";
  for (std::vector<const List*>::const_iterator i = m_deleteFromOrderListSoon.begin();
       i != m_deleteFromOrderListSoon.end(); ++i) {
    m_orderList.Delete(**i);
  }
  m_deleteFromOrderListSoon.clear();
  g_san.debug() << "Cleaning up node pool: " << m_nodePool;
  m_nodePool.Cleanup();
  g_san.debug() << "Cleaning up IList pool: " << m_ilistPool;
  m_ilistPool.Cleanup();
  SanityCheck("Cleanup");
}

void IncParser::InsertOutput(const OutputItem& item, Batch& out_batch, const List*& behind_node, State::Station worst_station) {
  if (item.onode) {
    g_log.debug() << "InsertOutput for item onode " << *item.onode;
    if (behind_node) {
      g_log.debug() << " - at pos " << *behind_node;
    }
    if (State::ST_PENDING == worst_station || State::ST_BAD == worst_station) {
      g_log.debug() << "NOT emitting onode " << *item.onode << " since worst station is " << worst_station;
      return;
    }
    out_batch.Insert(*item.onode, behind_node);
  } else {
    g_log.debug() << "InsertOutput for item child " << *item.child;
    ComputeOutput_Insert(*item.child, out_batch, behind_node, worst_station);
  }
}

void IncParser::RemoveOutput(const OutputItem& item, Batch& out_batch, State::Station worst_station) {
  if (item.onode) {
    //g_log.debug() << "RemoveOutput for item onode " << *item.onode; // UNSAFE
    out_batch.Delete(*item.onode);
  } else {
    //g_log.debug() << "RemoveOutput for item child " << *item.child; // UNSAFE
    ComputeOutput_Delete(*item.child, out_batch, worst_station);
  }
}

void IncParser::UpdateOutput(OutputItem& item, Batch& out_batch, const List*& behind_node, State::Station worst_station) {
  if (item.onode && item.onode->value != item.prev_value) {
    g_log.debug() << "UpdateOutput for item onode " << *item.onode;
    if (State::ST_PENDING == worst_station || State::ST_BAD == worst_station) {
      g_log.debug() << "NOT emitting update to onode " << *item.onode << " since worst station is " << worst_station;
      return;
    }
    item.prev_value = item.onode->value;
    out_batch.Update(*item.onode);
  } else if (item.child && m_touchedNodes.find(item.child) != m_touchedNodes.end()) {
    g_log.debug() << "UpdateOutput for item child " << *item.child;
    ComputeOutput_Update(*item.child, out_batch, behind_node, worst_station);
  } else {
    g_log.debug() << " - nothing to output for item " << item;
  }
}

const List* IncParser::GetOEnd(const OutputItem& item) const {
  if (item.onode) {
    return item.onode;
  } else if (!item.child) {
    throw SError("Defective GetOEnd()");
  }
  return item.child->GetOutputFunc().OEnd();
}

void IncParser::SanityCheck(const std::string& why) {
  g_log.debug() << "Sanity check " << m_sancount << " for " << why;
  g_log.debug();
  g_san.info();
  g_san.info() << "Sanity check " << m_sancount;
  SanityCheck(&m_root, 0);
  g_san.debug() << "Sanity " << m_sancount << " is done.";
  ++m_sancount;
}

void IncParser::SanityCheck(const STree* s, int depth) const {
  string space;
  for (int i = 0; i < depth; ++i) {
    space += " - ";
  }
  g_san.info() << space << "SNode: " << s;
  g_san.info() << space << ":: " << *s;
  if (!s->GetIConnection().IsClear()) {
    const List* inode = &s->IStart();
    while (inode) {
      g_san.info() << space << " - INode: " << inode;
      g_san.info() << space <<" - :: " << *inode;
      if (inode == &s->IEnd()) {
        break;
      }
      inode = inode->right;
    }
  }
  for (STree::child_iter child = s->children.begin(); child != s->children.end(); ++child) {
    g_san.info() << space << " - child: " << *child;
    g_san.info() << space << " - :: " << **child;
  }
  for (STree::child_iter child = s->children.begin(); child != s->children.end(); ++child) {
    if ((*child)->GetParent() != s) {
      g_san.info() << space << " - child " << *child << " parent check fail; child's parent is: " << (*child)->GetParent();
    }
  }
  for (STree::child_iter child = s->children.begin(); child != s->children.end(); ++child) {
    g_san.info() << space << s << " (" << *s << ") -> " << *child;
    SanityCheck(*child, depth+1);
  }
  // Can't output OList; something something deleted something
/*
  const List* onode = s->GetOutputFunc().OStart();
  while (onode) {
    g_san.info() << space << " - ONode: " << onode;
    g_san.info() << space << " - :: " << *onode;
    if (onode == s->GetOutputFunc().OEnd()) {
      break;
    }
    onode = onode->right;
  }
*/
  g_san.info() << space << "Sanity done for " << s;
}
