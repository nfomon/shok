// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Grapher.h"

#include "Connector.h"
#include "FWError.h"
#include "FWTree.h"
#include "IList.h"
#include "Rule.h"

#include "util/Graphviz.h"
#include "util/Util.h"
using Util::dotVar;

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <iostream>
#include <string>
using std::string;

using namespace fw;

/* public */

void Grapher::AddMachine(const std::string& context, const Rule& machineRoot) {
  m_graph += "subgraph cluster_" + dotVar(&machineRoot, context) + " {\n";
  m_graph += "node [shape=box];\n";
  m_graph += "graph [style=\"filled\", fillcolor=\"#f0f0f0\"];\n";
  m_graph += machineRoot.DrawNode(context);
  m_graph += "}\n";
  m_isDirty = true;
}

void Grapher::AddIList(const std::string& context, const IList& inode, const std::string& label) {
  m_graph += "subgraph cluster_" + context + dotVar(&inode, context) + " {\n";
  if (!label.empty()) {
    m_graph += "label=\"" + label + "\";\n";
    m_graph += "fontsize=12.0;\n";
  }
  m_graph += inode.DrawNode(context);
  m_graph += "}\n";
  m_isDirty = true;
}

void Grapher::AddOTree(const std::string& context, const FWTree& onode, const std::string& label) {
  m_graph += "subgraph cluster_" + context + dotVar(&onode, context) + " {\n";
  if (!label.empty()) {
    m_graph += "label=\"" + label + "\";\n";
    m_graph += "fontsize=12.0;\n";
  }
  m_graph += onode.DrawNode(context);
  m_graph += "}\n";
  m_isDirty = true;
}

void Grapher::AddIListeners(const std::string& context, const Connector& connector, const IList& inode) {
  Connector::listener_set ls = connector.GetListeners(inode);
  for (Connector::listener_iter i = ls.begin(); i != ls.end(); ++i) {
    m_graph += dotVar(&inode, context) + " -> " + dotVar(*i, context) + " [constraint=false, weight=0, style=dashed, arrowsize=0.5, color=\"#3333cc\"];\n";
  }
  if (inode.right) {
    AddIListeners(context, connector, *inode.right);
  }
  m_isDirty = true;
}

void Grapher::Signal(const std::string& context, const void* x, bool isUpdate) {
  string color = "#dd3300";
  if (isUpdate) {
    color = "#003399";
  }
  m_graph += dotVar(x, context) + " [color=\"" + color + "\", penwidth=2.0];\n";
  m_isDirty = true;
}

void Grapher::Save() {
  if (!m_isDirty) {
    return;
  }
  m_graph += "}\n";
  std::string outfile(m_out_dir + "/" + m_base_filename + Util::pad_str(lexical_cast<string>(m_img_count), '0', 2) + ".dot");
  std::ofstream out(outfile.c_str());
  if (!out) {
    throw FWError("Failed to open output graph file " + outfile);
  }
  out << m_graph;
  out.close();
  ++m_img_count;
}

void Grapher::Clear() {
  m_isDirty = false;
  m_graph = "digraph {\n";
  m_graph += "graph [compound=true, splines=line, dpi=65];\n";
}
