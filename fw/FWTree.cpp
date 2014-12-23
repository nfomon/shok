// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "FWTree.h"

#include "util/Graphviz.h"
using Util::dotVar;

#include <string>
using std::string;

using namespace fw;

string FWTree::DrawNode(const string& context) const {
  string s;
  // Style the node to indicate its State
  string fillcolor = "#88ffaa";
  if (m_state->IsBad()) {
    fillcolor = "#ff9999";
  } else if (m_state->IsDone()) {
    fillcolor = "#eeee66";
  } else if (m_state->IsComplete()) {
    fillcolor = "#9999cc";
  }

  s += dotVar(this, context) + " [label=\"" + Util::safeLabelStr(m_state->rule.Name()) + "\", style=\"filled\", fillcolor=\"" + fillcolor + "\", fontsize=12.0];\n";
  // Connect the node to its IConnection
  string istartcolor = "#006600";
  string iendcolor = "#660000";
  if (m_state->IsBad()) {
    istartcolor = "#66cc66";
    iendcolor = "#cc6666";
  }
  if (iconnection.istart) {
    s += dotVar(this, context) + " -> " + dotVar(iconnection.istart, context) + " [constraint=false, weight=0, style=dotted, arrowsize=0.5, color=\"" + istartcolor + "\"];\n";
  } else {
    s += dotVar(this, context) + " -> " + dotVar(this, context) + " [constraint=false, weight=0, style=dotted, arrowsize=0.5, color=\"" + istartcolor + "\"];\n";
  }
  if (iconnection.iend) {
    s += dotVar(this, context) + " -> " + dotVar(iconnection.iend, context) + " [constraint=false, weight=0, style=dotted, arrowsize=0.5, color=\"" + iendcolor + "\"];\n";
  } else {
    s += dotVar(this, context) + " -> " + dotVar(this, context) + " [constraint=false, weight=0, style=dotted, arrowsize=0.5, color=\"" + iendcolor + "\"];\n";
  }
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
  s += m_oconnection->DrawOConnection(context);
  return s;
}
