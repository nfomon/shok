// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "IncParser.h"

#include "Grapher.h"
#include "Hotlist.h"
#include "IList.h"
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

IncParser::IncParser(Rule& grammar, const string& name, const string& graphdir)
  : m_rootRule(name, MakeComputeFunc_Root(name), MakeOutputFunc_Pass()),
    m_root(*this, m_rootRule, NULL, MakeComputeFunc_Root(name), MakeOutputFunc_Pass()),
    m_grammar(grammar),
    m_name(name),
    m_needsCleanup(false),
    m_sancount(0) {
  m_rootRule.AddChildRecursive(&m_grammar); // not "recursive", just unowned
  if (!graphdir.empty()) {
    m_grapher.reset(new Grapher(graphdir, string(name + "_")));
    m_grapher->AddMachine(name, m_rootRule);
    m_grapher->SaveAndClear();
  }
}

void IncParser::ExtractHotlist(Hotlist& out_hotlist) {
  g_log.debug() << "EXTRACT START: " << m_name;
  if (m_touchedNodes.find(&m_root) != m_touchedNodes.end()) {
    ComputeOutput_Update(m_root, out_hotlist);
  }
  g_log.debug() << "EXTRACT DONE: " << m_name;
  m_touchedNodes.clear();
  m_needsCleanup = true;
  if (!out_hotlist.IsEmpty()) {
    DrawGraph(m_root, /*inode*/ NULL, &out_hotlist);
  }
}

IncParser::listener_set IncParser::GetListeners(const IList& x) const {
  return m_listeners.GetListeners(&x);
}

void IncParser::Insert(const IList& inode) {
  Hotlist hotlist;
  hotlist.Insert(inode);
  UpdateWithHotlist(hotlist.GetHotlist());
}

void IncParser::Delete(const IList& inode) {
  Hotlist hotlist;
  hotlist.Delete(inode);
  UpdateWithHotlist(hotlist.GetHotlist());
}

void IncParser::Update(const IList& inode) {
  Hotlist hotlist;
  hotlist.Update(inode);
  UpdateWithHotlist(hotlist.GetHotlist());
}

void IncParser::UpdateWithHotlist(const Hotlist::hotlist_vec& hotlist) {
  g_log.info() << "Updating IncParser " << m_name << " with hotlist of size " << hotlist.size();
  CleanupIfNeeded();
  for (Hotlist::hotlist_iter i = hotlist.begin(); i != hotlist.end(); ++i) {
    switch (i->second) {
    case Hotlist::OP_INSERT:
      InsertNode(*i->first);
      break;
    case Hotlist::OP_DELETE:
      DeleteNode(*i->first);
      break;
    case Hotlist::OP_UPDATE:
      UpdateNode(*i->first);
      break;
    default:
      throw SError("Cannot update with hotlist with unknown hot operation");
    }
  }
  g_log.info() << "Done updating IncParser " << m_name << " with hotlist that had size " << hotlist.size();
}

void IncParser::Enqueue(ParseAction action) {
  g_log.info() << "IncParser " << m_name << ": Enqueuing action " << ParseAction::UnMapAction(action.action) << " - " << *action.node << " at depth " << action.node->depth;
  int depth = action.node->depth;
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

void IncParser::ClearNode(STree& x) {
  m_listeners.RemoveAllListenings(&x);
  if (&x != &m_root) {
    g_san.debug() << "xx Unlinking node " << x << " - " << &x;
    m_nodePool.Unlink(x);
  }
}

void IncParser::Listen(STree& x, const IList& inode) {
  if (m_listeners.IsListening(&inode, &x)) {
    g_log.info() << "IncParser: " << x << " is already listening to " << inode;
  } else {
    g_log.info() << "IncParser: " << x << " will listen to " << inode;
    m_listeners.AddListener(&inode, &x);
  }
}

void IncParser::Unlisten(STree& x, const IList& inode) {
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

const IList* IncParser::GetFirstINode() const {
  if (m_root.IsClear() || m_root.GetIConnection().IsClear()) {
    return NULL;
  }
  return &m_root.IStart();
}

const IList* IncParser::GetFirstONode() const {
  return m_root.GetOutputFunc().OStart();
}

void IncParser::TouchNode(const STree& node) {
  m_touchedNodes.insert(&node);
}

void IncParser::DrawGraph(const STree& onode, const IList* inode, const Hotlist* hotlist, const STree* initiator) {
  if (!m_grapher.get()) {
    return;
  }
  SanityCheck();
  const IList* istart = GetFirstINode();
  if (istart) {
    m_grapher->AddIList(m_name, *istart, m_name + " input");
  }
  m_grapher->AddSTree(m_name, m_root, m_name + " output", initiator);
  const IList* ostart = GetFirstONode();
  if (ostart) {
    m_grapher->AddOList(m_name, *ostart, m_name + " olist");
  }
  if (hotlist) {
    g_log.debug() << "Drawing hotlist: " << hotlist->Print();
    m_grapher->AddHotlist(m_name, hotlist->GetHotlist());
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
    const IList* inode = &s->IStart();
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
  const IList* onode = s->GetOutputFunc().OStart();
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

/* private */

const STree& IncParser::GetRoot() const {
  return m_root;
}

void IncParser::InsertNode(const IList& inode) {
  g_log.info() << "IncParser " << m_name << ": Inserting IList: " << inode << " with " << (inode.left ? "left" : "no left") << " and " << (inode.right ? "with a right" : "no right");
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
        //if ((*i)->GetState().IsBad() || (*i)->GetState().IsComplete()) {
        //  g_log.debug() << "Not enqueueing INodeInsert for left listener " << **i << " because it is bad or complete";
        //} else {
          Enqueue(ParseAction(ParseAction::INodeInsert, **i, inode));
        //}
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

void IncParser::DeleteNode(const IList& inode) {
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

void IncParser::UpdateNode(const IList& inode) {
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
        a->node->ComputeNode(a->action, *a->inode, a->initiator);
      }
    }
    if (m_actions_by_depth.at(depth).empty()) {
      g_log.debug() << "IncParser " << m_name << " Clearing actions at depth " << depth;
      m_actions_by_depth.erase(depth);
    }
  }
}

void IncParser::ComputeOutput_Update(const STree& node, Hotlist& out_hotlist) {
  g_log.debug() << "IncParser " << m_name << ": Computing output UPDATE for node " << node;

  node.GetOutputFunc()();
  const OutputState& os = node.GetOutputFunc().GetState();
  output_mod_iter posi = m_outputPerNode.find(&node);

  if (m_outputPerNode.end() == posi) {
    // We weren't emitting before, but are emitting now.  Insert all.
    g_log.debug() << " - was emitting nothing; inserting all";
    return ComputeOutput_Insert(node, out_hotlist);
  }

  OutputState& pos = posi->second;
  g_log.debug() << node << " was emitting " << pos.onodes.size() << " ONodes and " << pos.children.size() << " children";
  g_log.debug() << node << " is now emitting " << os.onodes.size() << " ONodes and " << os.children.size() << " children";
  // ONodes
  for (OutputState::onode_iter onode = os.onodes.begin(); onode != os.onodes.end(); ++onode) {
    OutputState::onode_iter ponode = pos.onodes.find(*onode);
    if (pos.onodes.end() == ponode) {
      g_log.debug() << node << " WAS NOT EMITTING node " << **onode << "; inserting now.";
      out_hotlist.Insert(**onode);
    } else {
      g_log.debug() << node << " WAS EMITTING node " << **onode << "; updating, and erasing from pos.";
      pos.onodes.erase(ponode);
      if (os.value != pos.value) {
        out_hotlist.Update(**onode);
      }
    }
  }
  for (OutputState::onode_iter ponode = pos.onodes.begin(); ponode != pos.onodes.end(); ++ponode) {
    g_log.debug() << "Update causes deletion of ponode " << **ponode;
    out_hotlist.Delete(**ponode);
  }

  // Children
  for (OutputState::child_iter child = os.children.begin(); child != os.children.end(); ++child) {
    OutputState::child_iter pchild = pos.children.find(*child);
    if (pos.children.end() == pchild) {
      g_log.debug() << node << " WAS NOT EMITTING child " << **child << "; inserting now.";
      ComputeOutput_Insert(**child, out_hotlist);
    } else {
      g_log.debug() << node << " WAS EMITTING child " << **child << "; updating child.";
      pos.children.erase(pchild);
      if (m_touchedNodes.find(*child) != m_touchedNodes.end()) {
        ComputeOutput_Update(**child, out_hotlist);
      }
    }
  }

  for (OutputState::child_iter pchild = pos.children.begin(); pchild != pos.children.end(); ++pchild) {
    g_log.debug() << "Update causes deletion of pchild " << **pchild;
    ComputeOutput_Delete(**pchild, out_hotlist);
  }

  if (os.onodes.empty() && os.children.empty()) {
    m_outputPerNode.erase(&node);
  } else {
    m_outputPerNode[&node] = os;   // copy
  }
  g_log.debug() << "Done computing UPDATE output for node " << node << ".  Hotlist so far: " << out_hotlist.Print();
  g_log.debug() << " - have " << os.onodes.size() << " ONodes and " << os.children.size() << " children";
  node.GetOutputFunc().ConnectONodes();
}

void IncParser::ComputeOutput_Insert(const STree& node, Hotlist& out_hotlist) {
  g_log.debug() << "IncParser " << m_name << ": Computing output INSERT for node " << node;

  node.GetOutputFunc()();
  const OutputState& os = node.GetOutputFunc().GetState();

  // Just insert everything, disregard what the prev output says
  for (OutputState::onode_iter onode = os.onodes.begin(); onode != os.onodes.end(); ++onode) {
    out_hotlist.Insert(**onode);
  }
  for (OutputState::child_iter child = os.children.begin(); child != os.children.end(); ++child) {
    ComputeOutput_Insert(**child, out_hotlist);
  }

  if (os.onodes.empty() && os.children.empty()) {
    m_outputPerNode.erase(&node);
  } else {
    m_outputPerNode[&node] = os;   // copy
  }
  g_log.debug() << "Done computing INSERT output for node " << node << ".  Hotlist so far: " << out_hotlist.Print();
  g_log.debug() << m_name << " node " << node << " is now emitting " << os.onodes.size() << " ONodes and " << os.children.size() << " children.";
  node.GetOutputFunc().ConnectONodes();
}

void IncParser::ComputeOutput_Delete(const STree& node, Hotlist& out_hotlist) {
  g_log.debug() << "IncParser " << m_name << ": Computing output DELETE for node " << node;

  output_mod_iter posi = m_outputPerNode.find(&node);
  if (m_outputPerNode.end() == posi) {
    return;
  }

  // Just delete everything from prev, don't compute its current output
  const OutputState& pos = posi->second;
  for (OutputState::onode_iter ponode = pos.onodes.begin(); ponode != pos.onodes.end(); ++ponode) {
    out_hotlist.Delete(**ponode);
  }
  for (OutputState::child_iter child = pos.children.begin(); child != pos.children.end(); ++child) {
    ComputeOutput_Delete(**child, out_hotlist);
  }
  m_outputPerNode.erase(&node);
  g_log.debug() << "Done computing DELETE output for node " << node << ".  Hotlist so far: " << out_hotlist.Print();
}

void IncParser::CleanupIfNeeded() {
  if (m_needsCleanup) {
    g_log.debug() << "IncParser " << m_name << " - cleaning up";
    g_san.debug() << "Cleaning up pool: " << string(m_nodePool);
    m_nodePool.Cleanup();
    m_needsCleanup = false;
    SanityCheck();
  }
}

int IncParser::INodeCompare(const IList& a, const IList& b) const {
  return m_orderList.Compare(a, b);
}