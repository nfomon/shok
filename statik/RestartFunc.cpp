// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "RestartFunc.h"

#include "IList.h"
#include "Rule.h"
#include "STree.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace statik;

auto_ptr<RestartFunc> statik::MakeRestartFunc_Default() {
  return auto_ptr<RestartFunc>(new RestartFunc_Default());
}

auto_ptr<RestartFunc> statik::MakeRestartFunc_KeepAll() {
  return auto_ptr<RestartFunc>(new RestartFunc_KeepAll());
}

auto_ptr<RestartFunc> statik::MakeRestartFunc_Sequence() {
  return auto_ptr<RestartFunc>(new RestartFunc_Sequence());
}

auto_ptr<RestartFunc> statik::MakeRestartFunc_AllChildrenOfNode() {
  return auto_ptr<RestartFunc>(new RestartFunc_AllChildrenOfNode());
}

void RestartFunc_Default::operator() (const IList& istart) {
  if (!m_node->GetRule().GetChildren().empty()) {
    throw SError("Node " + string(*m_node) + " is using RestartFunc_Default but its Rule has children");
  } else if (!m_node->children.empty()) {
    throw SError("Node " + string(*m_node) + " is using RestartFunc_Default but it has children");
  }
  m_node->GetConnector().UnlistenAll(*m_node);
  g_log.info() << "Default RestartFunc is enqueueing its node " << *m_node << " for compute";
  m_node->GetConnector().Enqueue(ConnectorAction(ConnectorAction::Compute, *m_node, istart));
}

void RestartFunc_Sequence::operator() (const IList& istart) {
  g_log.info() << "Restarting " << *m_node << " as sequence, with istart=" << istart;
  if (!m_node) {
    throw SError("Cannot compute uninitialized RestartFunc_Sequence");
  }
  if (m_node->GetRule().GetChildren().size() < 1) {
    throw SError("Rule " + m_node->GetRule().Name() + " of node " + string(*m_node) + " must have at least one child for RestartFunc_Sequence");
  }

  // Prepend as our new first child
  m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, istart, m_node->children.begin());
}

void RestartFunc_KeepAll::operator() (const IList& istart) {
  g_log.info() << "Restarting " << *m_node << " to first child, keeping all children, with istart=" << istart;
  if (!m_node) {
    throw SError("Cannot compute uninitialized RestartFunc_KeepAll");
  }
  if (m_node->GetRule().GetChildren().size() < 1) {
    throw SError("Rule " + m_node->GetRule().Name() + " of node " + string(*m_node) + " must have at least one child for RestartFunc_KeepAll");
  }

  m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, istart, m_node->children.begin());
}

void RestartFunc_AllChildrenOfNode::operator() (const IList& istart) {
  g_log.info() << "Restarting all children of " << *m_node << ", with istart=" << istart;
  if (!m_node) {
    throw SError("Cannot compute uninitialized RestartFunc_AllChildrenOfNode");
  }
  if (m_node->children.empty()) {
    for (Rule::child_iter i = m_node->GetRule().GetChildren().begin();
        i != m_node->GetRule().GetChildren().end(); ++i) {
      (*i)->MakeNode(*m_node, istart);
    }
  } else if (m_node->children.size() == m_node->GetRule().GetChildren().size()) {
    for (STree::child_mod_iter i = m_node->children.begin();
         i != m_node->children.end(); ++i) {
      (*i)->GetConnector().Enqueue(ConnectorAction(ConnectorAction::Restart, **i, istart));
    }
  } else {
    throw SError("Rule " + m_node->GetRule().Name() + " of node " + string(*m_node) + " has inappropriate # of children for RepositionAllChildren");
  }
}
