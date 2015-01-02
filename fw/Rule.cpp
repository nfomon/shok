// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Rule.h"

#include "Connector.h"
#include "FWError.h"
#include "FWTree.h"
#include "Keyword.h"
#include "Meta.h"
#include "Or.h"
#include "Regexp.h"
#include "Seq.h"
#include "Star.h"

#include "util/Graphviz.h"
using Util::dotVar;
using Util::safeLabelStr;

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace fw;

Rule::Rule(Log& log, const string& debugName, auto_ptr<RestartFunc> restartFunc, auto_ptr<ComputeFunc> computeFunc, auto_ptr<OutputFunc> outputFunc)
  : m_log(log),
    m_name(debugName),
    m_restartFunc(restartFunc),
    m_computeFunc(computeFunc),
    m_outputFunc(outputFunc),
    m_parent(NULL) {
}

Rule& Rule::SetRestartFunc(auto_ptr<RestartFunc> restartFunc) {
  m_restartFunc = restartFunc;
  return *this;
}

Rule& Rule::SetComputeFunc(auto_ptr<ComputeFunc> computeFunc) {
  m_computeFunc = computeFunc;
  return *this;
}

Rule& Rule::SetOutputFunc(auto_ptr<OutputFunc> outputFunc) {
  m_outputFunc = outputFunc;
  return *this;
}

void Rule::SilenceOutput() {
  m_outputFunc = MakeOutputFunc_Silent(m_log);
}

void Rule::CapOutput(const string& cap) {
  m_outputFunc = MakeOutputFunc_Cap(m_log, m_outputFunc, cap);
}

auto_ptr<FWTree> Rule::MakeRootNode(Connector& connector) const {
  auto_ptr<FWTree> node(new FWTree(m_log, connector, *this, NULL, m_restartFunc->Clone(), m_computeFunc->Clone(), m_outputFunc->Clone()));
  return node;
}

FWTree* Rule::MakeNode(FWTree& parent, const IList& istart) const {
  auto_ptr<FWTree> node(new FWTree(m_log, parent.GetConnector(), *this, &parent, m_restartFunc->Clone(), m_computeFunc->Clone(), m_outputFunc->Clone()));
  FWTree* r = node.get();
  parent.children.push_back(node);
  r->RestartNode(istart);
  return r;
}

Rule* Rule::AddChild(auto_ptr<Rule> child) {
  child->setParent(this);
  m_children.push_back(child);
  return &m_children.back();
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

string Rule::DrawNode(const string& context) const {
  string s;
  s += dotVar(this, context) + " [label=\"" + safeLabelStr(m_name) + "\"];\n";
  for (child_iter i = m_children.begin(); i != m_children.end(); ++i) {
    s += dotVar(this, context) + " -> " + dotVar(&*i, context) + ";\n";
    s += i->DrawNode(context);
  }
  return s;
}
