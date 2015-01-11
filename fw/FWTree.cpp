// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "FWTree.h"

#include "util/Graphviz.h"
#include "util/Log.h"
using Util::dotVar;

#include <memory>
#include <ostream>
#include <string>
using std::auto_ptr;
using std::ostream;
using std::string;

using namespace fw;

/* public */

FWTree::FWTree(Connector& connector,
               const Rule& rule, FWTree* parent,
               auto_ptr<RestartFunc> restartFunc,
               auto_ptr<ComputeFunc> computeFunc,
               auto_ptr<OutputFunc> outputFunc)
  : m_connector(connector),
    m_rule(rule),
    m_parent(parent),
    depth(m_parent ? m_parent->depth + 1 : 0),
    m_restartFunc(restartFunc),
    m_computeFunc(computeFunc),
    m_outputFunc(outputFunc) {
  m_restartFunc->Init(*this);
  m_computeFunc->Init(*this);
  m_outputFunc->Init(*this);
}

void FWTree::RestartNode(const IList& istart) {
  g_log.info() << "Restarting node " << *this << " with inode " << istart;
  m_iconnection.Restart(istart);
  m_connector.DrawGraph(*this, &istart);
  m_state.Unlock();
  Restart(istart);
  (void) ComputeNode();
}

bool FWTree::ComputeNode() {
  g_log.info() << "Updating node " << *this;
  const IList& old_iend = IEnd();
  (*m_computeFunc)();
  (*m_outputFunc)();
  m_connector.DrawGraph(*this);
  bool hasChanged = &old_iend != &IEnd() || !m_outputFunc->GetHotlist().empty();
  g_log.debug() << " - - - - " << *this << " has " << (hasChanged ? "" : "NOT ") << "changed";
  return hasChanged;
}

void FWTree::ClearNode() {
  g_log.info() << "Clearing node " << *this;
  for (child_mod_iter i = children.begin(); i != children.end(); ++i) {
    i->ClearNode();
  }
  m_state.Clear();
  m_connector.ClearNode(*this);
}

FWTree::operator std::string() const {
  return m_rule.Name() + ":" + string(GetState());
}

string FWTree::DrawNode(const string& context) const {
  string s;
  // Style the node to indicate its State
  string fillcolor = "#ffeebb";
  if (m_state.IsBad()) {
    fillcolor = "#ff8888";
  } else if (m_state.IsDone()) {
    fillcolor = "#99ffcc";
  } else if (m_state.IsComplete()) {
    fillcolor = "#aaaaff";
  }
  s += dotVar(this, context) + " [label=\"" + Util::safeLabelStr(m_rule.Name()) + "\", style=\"filled\", fillcolor=\"" + fillcolor + "\", fontsize=12.0];\n";

  // Connect the node to its IConnection
  string istartcolor = "#006600";
  string iendcolor = "#660000";
  if (m_state.IsBad()) {
    istartcolor = "#66cc66";
    iendcolor = "#cc6666";
  }
  s += dotVar(this, context) + " -> " + dotVar(&m_iconnection.Start(), context) + " [constraint=false, weight=0, style=dotted, arrowsize=0.5, color=\"" + istartcolor + "\"];\n";
  //s += dotVar(this, context) + " -> " + dotVar(this, context) + " [constraint=false, weight=0, style=dotted, arrowsize=0.5, color=\"" + istartcolor + "\"];\n";
  s += dotVar(this, context) + " -> " + dotVar(&m_iconnection.End(), context) + " [constraint=false, weight=0, style=dotted, arrowsize=0.5, color=\"" + iendcolor + "\"];\n";
  //s += dotVar(this, context) + " -> " + dotVar(this, context) + " [constraint=false, weight=0, style=dotted, arrowsize=0.5, color=\"" + iendcolor + "\"];\n";

  // Add its child connections, and draw the children
  // Make sure the children will be ordered in the output graph
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    s += dotVar(this, context) + " -> " + dotVar(&*i, context) + ";\n";
  }
  if (children.size() > 0) {
    s += "{ rank=same;\n";
    child_iter prev_child = children.begin();
    for (child_iter i = children.begin() + 1; i != children.end(); ++i) {
      s += dotVar(&*prev_child, context) + " -> " + dotVar(&*i, context) + " [style=\"invis\"];\n";
      prev_child = i;
    }
    s += "}\n";
  }
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    s += i->DrawNode(context);
  }
  //s += m_outputFunc->DrawEmitting(context, *this);
  return s;
}

/* private */

void FWTree::Restart(const IList& istart) {
  (*m_restartFunc.get())(istart);
}

/* non-member */

ostream& fw::operator<< (ostream& out, const FWTree& node) {
  out << string(node);
  return out;
}
