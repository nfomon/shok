// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "RestartFunc.h"

#include "Connector.h"
#include "FWTree.h"
#include "IList.h"
#include "Rule.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace fw;

void RestartFunc_FirstChildOfNode::operator() (const IList& istart) {
  if (m_node.GetRule().GetChildren().size() < 1) {
    throw FWError("Rule " + string(m_node.GetRule()) + " of node " + string(m_node) + " must have at least one child for RestartFunc_FirstChildOfNode");
  }

  if (m_node.children.empty()) {
    m_node.GetRule().GetChildren().at(0).MakeNode(m_node, istart);
    return;
  }
  if (m_node.children.size() > 1) {
    // Remove all children > 1
    for (FWTree::child_mod_iter i = m_node.children.begin() + 1;
         i != m_node.children.end(); ++i) {
      i->ClearNode();
    }
    m_node.children.erase(m_node.children.begin() + 1, m_node.children.end());
  }
  m_node.children.at(0).RestartNode(istart);
}

void RestartFunc_AllChildrenOfNode::operator() (const IList& istart) {
  if (m_node.children.empty()) {
    for (Rule::child_iter i = m_node.GetRule().GetChildren().begin();
        i != m_node.GetRule().GetChildren().end(); ++i) {
      i->MakeNode(m_node, istart);
    }
  } else if (m_node.children.size() == m_node.GetRule().GetChildren().size()) {
    for (FWTree::child_mod_iter i = m_node.children.begin();
         i != m_node.children.end(); ++i) {
      i->RestartNode(istart);
    }
  } else {
    throw FWError("Rule " + string(m_node.GetRule()) + " of node " + string(m_node) + " has inappropriate # of children for RepositionAllChildren");
  }
}
