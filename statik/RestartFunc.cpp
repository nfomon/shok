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

auto_ptr<RestartFunc> statik::MakeRestartFunc_None() {
  return auto_ptr<RestartFunc>(new RestartFunc_None());
}

auto_ptr<RestartFunc> statik::MakeRestartFunc_KeepAll() {
  return auto_ptr<RestartFunc>(new RestartFunc_KeepAll());
}

auto_ptr<RestartFunc> statik::MakeRestartFunc_FirstChildOfNode() {
  return auto_ptr<RestartFunc>(new RestartFunc_FirstChildOfNode());
}

auto_ptr<RestartFunc> statik::MakeRestartFunc_AllChildrenOfNode() {
  return auto_ptr<RestartFunc>(new RestartFunc_AllChildrenOfNode());
}

void RestartFunc_FirstChildOfNode::operator() (const IList& istart) {
  g_log.info() << "Restarting " << *m_node << " to first child, with istart=" << istart;
  if (!m_node) {
    throw SError("Cannot compute uninitialized RestartFunc_FirstChildOfNode");
  }
  if (m_node->GetRule().GetChildren().size() < 1) {
    throw SError("Rule " + m_node->GetRule().Name() + " of node " + string(*m_node) + " must have at least one child for RestartFunc_FirstChildOfNode");
  }

  if (m_node->children.empty()) {
    m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, istart);
    return;
  }
  if (m_node->children.size() > 1) {
    // Remove all children > 1
    for (STree::child_mod_iter i = m_node->children.begin() + 1;
         i != m_node->children.end(); ++i) {
      (*i)->ClearNode();
    }
    m_node->children.erase(m_node->children.begin() + 1, m_node->children.end());
  }
  m_node->children.at(0)->RestartNode(istart);
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
      (*i)->RestartNode(istart);
    }
  } else {
    throw SError("Rule " + m_node->GetRule().Name() + " of node " + string(*m_node) + " has inappropriate # of children for RepositionAllChildren");
  }
}
