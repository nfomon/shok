// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Rule.h"

#include "Connector.h"
#include "FWTree.h"
#include "IList.h"

#include "util/Graphviz.h"
using Util::dotVar;
using Util::safeLabelStr;

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace fw;

string Rule::Print() const {
  string s(m_name);
  if (!m_children.empty()) {
    s += " (";
    for (child_iter i = m_children.begin(); i != m_children.end(); ++i) {
      if (i != m_children.begin()) { s += ", "; }
      s += i->Print();
    }
    s += ")";
  }
  return s;
}

string Rule::DrawNode(const std::string& context) const {
  string s;
  s += dotVar(this, context) + " [label=\"" + safeLabelStr(m_name) + "\"];\n";
  for (child_iter i = m_children.begin(); i != m_children.end(); ++i) {
    s += dotVar(this, context) + " -> " + dotVar(&*i, context) + ";\n";
    s += i->DrawNode(context);
  }
  return s;
}

void Rule::AddChildToNode(FWTree& x, const Rule& child) const {
  x.children.push_back(auto_ptr<FWTree>(new FWTree(child, &x)));
}

void Rule::RepositionFirstChildOfNode(Connector& connector, FWTree& x, const IList& inode) const {
  if (m_children.size() < 1) {
    throw FWError("Rule " + string(*this) + " at " + string(x) + " must have at least one child in order to RepositionFirstChild");
  }

  if (x.children.empty()) {
    AddChildToNode(x, m_children.at(0));
  } else if (x.children.size() > 1) {
    // Remove all children > 1
    for (FWTree::child_mod_iter i = x.children.begin() + 1;
         i != x.children.end(); ++i) {
      connector.ClearNode(*i);
    }
    x.children.erase(x.children.begin() + 1, x.children.end());
  }
  connector.RepositionNode(x.children.at(0), inode);
}

void Rule::RepositionAllChildrenOfNode(Connector& connector, FWTree& x, const IList& inode) const {
  if (x.children.empty()) {
    for (child_iter i = m_children.begin(); i != m_children.end(); ++i) {
      AddChildToNode(x, *i);
      connector.RepositionNode(x.children.back(), inode);
    }
  } else if (x.children.size() == m_children.size()) {
    for (FWTree::child_mod_iter i = x.children.begin();
         i != x.children.end(); ++i) {
      connector.RepositionNode(*i, inode);
    }
  } else {
    throw FWError("Rule " + string(*this) + " at " + string(x) + " has inappropriate # of children for RepositionAllChildren");
  }
}
