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

auto_ptr<OutputStrategy> Rule::MakeOutputStrategy(const FWTree& x) const {
  switch (m_outputStrategyType) {
  case OS_SINGLE: return auto_ptr<OutputStrategy>(new OutputStrategySingle(m_log, x));
  case OS_VALUE: return auto_ptr<OutputStrategy>(new OutputStrategyValue(m_log, x));
  case OS_WINNER: return auto_ptr<OutputStrategy>(new OutputStrategyWinner(m_log, x));
  case OS_SEQUENCE: return auto_ptr<OutputStrategy>(new OutputStrategySequence(m_log, x));
  default: throw FWError("Cannot make OutputStrategy for rule " + string(*this) + " node " + string(x) + "; unknown output strategy type");
  }
}

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

FWTree* Rule::AddChildToNode(FWTree& x, const Rule& childRule, const IList& istart) const {
  auto_ptr<FWTree> child(new FWTree(childRule, &x, istart));
  FWTree* r = child.get();
  x.children.push_back(child);
  return r;
}

void Rule::RepositionFirstChildOfNode(Connector& connector, FWTree& x, const IList& istart) const {
  if (m_children.size() < 1) {
    throw FWError("Rule " + string(*this) + " at " + string(x) + " must have at least one child in order to RepositionFirstChild");
  }

  if (x.children.empty()) {
    (void) AddChildToNode(x, m_children.at(0), istart);
  } else if (x.children.size() > 1) {
    // Remove all children > 1
    for (FWTree::child_mod_iter i = x.children.begin() + 1;
         i != x.children.end(); ++i) {
      connector.ClearNode(*i);
    }
    x.children.erase(x.children.begin() + 1, x.children.end());
  }
  connector.RepositionNode(x.children.at(0), istart);
}

void Rule::RepositionAllChildrenOfNode(Connector& connector, FWTree& x, const IList& istart) const {
  if (x.children.empty()) {
    for (child_iter i = m_children.begin(); i != m_children.end(); ++i) {
      FWTree* child = AddChildToNode(x, *i, istart);
      connector.RepositionNode(*child, istart);
    }
  } else if (x.children.size() == m_children.size()) {
    for (FWTree::child_mod_iter i = x.children.begin();
         i != x.children.end(); ++i) {
      connector.RepositionNode(*i, istart);
    }
  } else {
    throw FWError("Rule " + string(*this) + " at " + string(x) + " has inappropriate # of children for RepositionAllChildren");
  }
}
