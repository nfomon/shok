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

auto_ptr<FWTree> Rule::MakeRootNode(Connector& connector) const {
  auto_ptr<FWTree> node(new FWTree(m_log, connector, *this, NULL));
  node->Init(MakeRestartFunc(*node.get()),
             MakeOutputStrategy(*node.get()));
  return node;
}

FWTree* Rule::MakeNode(FWTree& parent, const IList& istart) const {
  auto_ptr<FWTree> node(new FWTree(m_log, parent.GetConnector(), *this, &parent));
  node->Init(MakeRestartFunc(*node.get()),
             MakeOutputStrategy(*node.get()));
  FWTree* r = node.get();
  parent.children.push_back(node);
  r->RestartNode(istart);
  return r;
}

auto_ptr<RestartFunc> Rule::MakeRestartFunc(FWTree& x) const {
  switch (m_restartFuncType) {
  case RF_None: return auto_ptr<RestartFunc>(new RestartFunc_None(m_log, x));
  case RF_FirstChildOfNode: return auto_ptr<RestartFunc>(new RestartFunc_FirstChildOfNode(m_log, x));
  case RF_AllChildrenOfNode: return auto_ptr<RestartFunc>(new RestartFunc_AllChildrenOfNode(m_log, x));
  default: throw FWError("Cannot make RestartFunc; unknown restart func");
  }
}

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
