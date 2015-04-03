// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Connector.h"

#include "Grapher.h"
#include "Hotlist.h"
#include "IList.h"
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

Connector::Connector(const Rule& rule, const string& name, const string& graphdir)
  : m_rule(rule),
    m_root(NULL),
    m_name(name),
    m_needsCleanup(false) {
  if (!graphdir.empty()) {
    m_grapher.reset(new Grapher(graphdir, string(name + "_")));
    m_grapher->AddMachine(name, m_rule);
    m_grapher->SaveAndClear();
  }
}

void Connector::ExtractHotlist(Hotlist& out_hotlist) {
  g_log.debug() << "EXTRACT START: " << m_name;
  ComputeOutput(m_root, COM_UPDATE, out_hotlist);
  g_log.debug() << "EXTRACT DONE: " << m_name;
  m_touchedNodes.clear();
  m_needsCleanup = true;
  if (!out_hotlist.IsEmpty()) {
    DrawGraph(*m_root, /*inode*/ NULL, &out_hotlist);
  }
}

Connector::listener_set Connector::GetListeners(const IList& x) const {
  return m_listeners.GetListeners(&x);
}

void Connector::Insert(const IList& inode) {
  Hotlist hotlist;
  hotlist.Insert(inode);
  UpdateWithHotlist(hotlist.GetHotlist());
}

void Connector::Delete(const IList& inode) {
  Hotlist hotlist;
  hotlist.Delete(inode);
  UpdateWithHotlist(hotlist.GetHotlist());
}

void Connector::Update(const IList& inode) {
  Hotlist hotlist;
  hotlist.Update(inode);
  UpdateWithHotlist(hotlist.GetHotlist());
}

void Connector::UpdateWithHotlist(const Hotlist::hotlist_vec& hotlist) {
  g_log.info() << "Updating Connector " << m_name << " with hotlist of size " << hotlist.size();
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
  g_log.info() << "Done updating Connector " << m_name << " with hotlist that had size " << hotlist.size();
}

void Connector::Enqueue(ConnectorAction action) {
  g_log.info() << "Connector " << m_name << ": Enqueuing action " << ConnectorAction::UnMapAction(action.action) << " - " << *action.node << " at depth " << action.node->depth;
  int depth = action.node->depth;
  action_map::iterator ai = m_actions_by_depth.find(depth);
  if (m_actions_by_depth.end() == ai) {
    action_queue actions;
    actions.push_back(action);
    m_actions_by_depth.insert(std::make_pair(depth, actions));
  } else {
    ai->second.push_back(action);
  }
}

void Connector::ClearNode(STree& x) {
  m_listeners.RemoveAllListenings(&x);
  if (&x != m_root) {
    m_nodePool.Unlink(x);
  }
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

void Connector::UnlistenAll(STree& x) {
  g_log.info() << "Connector " << m_name << ": Unlistening all from node " << x;
  m_listeners.RemoveAllListenings(&x);
}

STree* Connector::OwnNode(auto_ptr<STree> node) {
  g_log.debug() << "Connector " << m_name << ": Owning node " << string(*node);
  return m_nodePool.Insert(node);
}

const IList* Connector::GetFirstONode() const {
  if (!m_root) {
    return NULL;
  }
  return m_root->GetOutputFunc().OStart();
}

void Connector::TouchNode(const STree& node) {
  m_touchedNodes.insert(&node);
}

void Connector::DrawGraph(const STree& onode, const IList* inode, const Hotlist* hotlist) {
  if (!m_grapher.get()) {
    return;
  }
  if (!m_root) {
    return;
  }
  m_grapher->AddIList(m_name, m_root->IStart(), m_name + " input");
  m_grapher->AddSTree(m_name, *m_root, m_name + " output");
  const IList* ostart = GetFirstONode();
  if (ostart) {
    m_grapher->AddOList(m_name, *ostart, m_name + " olist");
    if (hotlist) {
      m_grapher->AddHotlist(m_name, hotlist->GetHotlist());
    }
  }
  m_grapher->AddIListeners(m_name, *this, m_root->IStart());
  if (inode) {
    m_grapher->Signal(m_name, &onode);
    m_grapher->Signal(m_name, inode);
  } else {
    m_grapher->Signal(m_name, &onode, true);
  }
  m_grapher->SaveAndClear();
}

/* private */

const STree& Connector::GetRoot() const {
  if (!m_root) {
    throw SError("Connector " + m_name + "; cannot get root node before it has been initialized");
  }
  return *m_root;
}

void Connector::InsertNode(const IList& inode) {
  g_log.info() << "Connector " << m_name << ": Inserting IList: " << inode << " with " << (inode.left ? "left" : "no left") << " and " << (inode.right ? "right" : "no right");

  if (!m_root) {
    g_log.debug() << " - No root; initializing the tree root";
    m_root = m_rule.MakeRootNode(*this);
  }
  if (m_root->IsClear()) {
    g_log.debug() << " - Clear root; restarting the tree root";
    Enqueue(ConnectorAction(ConnectorAction::Restart, *m_root, inode));
  } else {
    if (inode.left) {
      g_log.debug() << " - Found left inode " << *inode.left << ": updating listeners of " << inode << "'s left and (if present) right";
      listener_set listeners = m_listeners.GetListeners(inode.left);
      for (listener_iter i = listeners.begin(); i != listeners.end(); ++i) {
        Enqueue(ConnectorAction(ConnectorAction::INodeInsert, **i, inode));
      }
      if (inode.right) {
        listeners = m_listeners.GetListeners(inode.right);
        for (listener_iter i = listeners.begin(); i != listeners.end(); ++i) {
          Enqueue(ConnectorAction(ConnectorAction::INodeInsert, **i, inode));
        }
      }
    } else {
      g_log.debug() << " - No left inode: just restarting the root";
      Enqueue(ConnectorAction(ConnectorAction::Restart, *m_root, inode));
    }
  }

  ProcessActions();

  // Assert that the inode has at least one listener in the updated set.
  if (!m_listeners.HasAnyListeners(&inode)) {
    m_root->GetState().GoBad();
  }
  // Stronger check: every inode has at least one listener :)
  g_log.debug() << "Connector: Insert done";
}

void Connector::DeleteNode(const IList& inode) {
  g_log.info() << "Deleting list inode " << inode;

  if (!m_root) {
    throw SError("Connector " + m_name + ": cannot delete " + string(inode) + " when the root has not been initialized");
  }

  if (!inode.left && !inode.right) {
    m_root->ClearNode();
  } else {
    if (inode.left) {
      listener_set listeners = m_listeners.GetListeners(inode.left);
      for (listener_iter i = listeners.begin(); i != listeners.end(); ++i) {
        Enqueue(ConnectorAction(ConnectorAction::INodeDelete, **i, inode));
      }
    }
    listener_set listeners = m_listeners.GetListeners(&inode);
    for (listener_iter i = listeners.begin(); i != listeners.end(); ++i) {
      Enqueue(ConnectorAction(ConnectorAction::INodeDelete, **i, inode));
    }
  }

  m_listeners.RemoveAllListeners(&inode);
  g_log.debug() << "Connector: Delete done";
}

void Connector::UpdateNode(const IList& inode) {
  g_log.info() << "Connector " << m_name << ": Updating listeners of inode " << inode;
  listener_set listeners = m_listeners.GetListeners(&inode);
  for (listener_iter i = listeners.begin(); i != listeners.end(); ++i) {
    Enqueue(ConnectorAction(ConnectorAction::INodeUpdate, **i, inode));
  }
}

void Connector::ProcessActions() {
  while (!m_actions_by_depth.empty()) {
    STree::depth_t depth = m_actions_by_depth.rbegin()->first;
    action_queue& actions = m_actions_by_depth.rbegin()->second;
    g_log.debug() << "Connector " << m_name << ": Applying actions at depth " << depth;
    while (m_actions_by_depth.rbegin()->first == depth && !actions.empty()) {
      g_log.debug() << "Connector " << m_name << " * * Action loop iteration";
      ConnectorAction* a = &actions.front();
      actions.pop_front();
      if (ConnectorAction::Restart == a->action) {
        a->node->RestartNode(*a->inode);
      } else {
        a->node->ComputeNode(a->action, *a->inode, a->initiator);
      }
    }
    if (m_actions_by_depth.at(depth).empty()) {
      g_log.debug() << "Connector " << m_name << " Clearing actions at depth " << depth;
      m_actions_by_depth.erase(depth);
    }
  }
}

void Connector::ComputeOutput(const STree* node, ComputeOutputMode mode, Hotlist& out_hotlist) {
  g_log.debug() << "Connector " << m_name << ": Computing output for node " << *node << " with mode " << mode;
  // TODO do this check before we call ComputeOutput() on a node (including the root)
  if (m_touchedNodes.end() == m_touchedNodes.find(node)) {
    g_log.debug() << " - node " << *node << " has not been touched";
    return;
  }

  node->GetOutputFunc()();
  const OutputState& os = node->GetOutputFunc().GetState();

  output_mod_iter posi = m_outputPerNode.find(node);

  // debug
  if (m_outputPerNode.end() != posi) {
    OutputState& pos = posi->second;
    g_log.debug() << *node << " was emitting " << pos.onodes.size() << " onodes and " << pos.children.size() << " children";
    for (OutputState::onode_iter ponode = pos.onodes.begin(); ponode != pos.onodes.end(); ++ponode) {
      g_log.debug() << m_name << "    yyy   " << *node << "  -  prev ONode: " << **ponode;
    }
    for (OutputState::child_iter child = pos.children.begin(); child != pos.children.end(); ++child) {
      g_log.debug() << m_name << "    yyy    " << *node << "  -  prev child: " << **child;
    }
  }

  if (COM_UPDATE == mode) {
    if (m_outputPerNode.end() == posi) {
      // We weren't emitting before, but are emitting now.  Insert all.
      mode = COM_INSERT;
    } else {
      OutputState& pos = posi->second;
      for (OutputState::onode_iter onode = os.onodes.begin(); onode != os.onodes.end(); ++onode) {
        OutputState::onode_iter ponode = pos.onodes.find(*onode);
        if (pos.onodes.end() == ponode) {
          g_log.debug() << *node << " WAS NOT EMITTING node " << **onode << "; inserting now.";
          out_hotlist.Insert(**onode);
        } else {
          g_log.debug() << *node << " WAS EMITTING node " << **onode << "; updating, and erasing from pos.";
          pos.onodes.erase(ponode);
          if (os.value != pos.value) {
            out_hotlist.Update(**onode);
          }
        }
      }
      for (OutputState::onode_iter ponode = pos.onodes.begin(); ponode != pos.onodes.end(); ++ponode) {
        out_hotlist.Delete(**ponode);
      }
      for (OutputState::child_iter child = os.children.begin(); child != os.children.end(); ++child) {
        ComputeOutput(*child, COM_UPDATE, out_hotlist);
      }
    }
  }
  if (COM_INSERT == mode) {
    // Just insert everything, disregard what the prev output says
    for (OutputState::onode_iter onode = os.onodes.begin(); onode != os.onodes.end(); ++onode) {
      out_hotlist.Insert(**onode);
    }
    for (OutputState::child_iter child = os.children.begin(); child != os.children.end(); ++child) {
      ComputeOutput(*child, COM_INSERT, out_hotlist);
    }
  } else if (COM_DELETE == mode) {
    // Just delete everything from prev, disregard what the current output says
    const OutputState& pos = posi->second;
    for (OutputState::onode_iter ponode = pos.onodes.begin(); ponode != pos.onodes.end(); ++ponode) {
      out_hotlist.Delete(**ponode);
    }
    for (OutputState::child_iter child = pos.children.begin(); child != pos.children.end(); ++child) {
      ComputeOutput(*child, COM_DELETE, out_hotlist);
    }
  }

  g_log.debug() << "Done computing output for node " << *node << ".  Hotlist so far: " << out_hotlist.Print();
  for (OutputState::onode_iter onode = os.onodes.begin(); onode != os.onodes.end(); ++onode) {
    g_log.debug() << m_name << "    xxx   " << *node << "  -  ONode: " << **onode;
  }
  for (OutputState::child_iter child = os.children.begin(); child != os.children.end(); ++child) {
    g_log.debug() << m_name << "    xxx    " << *node << "  -  Child: " << **child;
  }
  m_outputPerNode[node] = os;   // copy
  g_log.debug() << m_name << " node " << *node << " is now emitting " << os.onodes.size() << " ONodes and " << os.children.size() << " children.";
  node->GetOutputFunc().ConnectONodes();
}

void Connector::CleanupIfNeeded() {
  if (m_needsCleanup) {
    g_log.debug() << "Connector " << m_name << " - cleaning up";
    m_nodePool.Cleanup();
    m_needsCleanup = false;
  }
}
