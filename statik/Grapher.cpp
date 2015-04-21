// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Grapher.h"

#include "Connector.h"
#include "IList.h"
#include "Rule.h"
#include "SError.h"
#include "SLog.h"
#include "STree.h"

#include "util/Graphviz.h"
#include "util/Util.h"
using Util::dotVar;

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <fstream>
#include <iostream>
#include <string>
using std::ofstream;
using std::string;

using namespace statik;

/* public */

Grapher::Grapher(const string& out_dir, const string& base_filename)
  : m_out_dir(out_dir),
    m_base_filename(base_filename),
    m_img_count(0),
    m_isDirty(false) {
  Clear();
}

void Grapher::AddMachine(const string& context, const Rule& machineRoot) {
  m_graph += "subgraph cluster_" + dotVar(&machineRoot, context) + " {\n";
  m_graph += "node [shape=box];\n";
  m_graph += "graph [style=\"filled\", fillcolor=\"#f0f0f0\"];\n";
  m_graph += machineRoot.DrawNode(context);
  m_graph += "}\n";
  m_isDirty = true;
}

void Grapher::AddIList(const string& context, const IList& inode, const string& label) {
  m_graph += "subgraph cluster_" + context + dotVar(&inode, context) + " {\n";
  if (!label.empty()) {
    m_graph += "label=\"" + label + "\";\n";
    m_graph += "fontsize=12.0;\n";
  }
  m_graph += inode.DrawNode(context);
  m_graph += "}\n";
  m_isDirty = true;
}

void Grapher::AddSTree(const string& context, const STree& snode, const string& label, const STree* initiator) {
  m_graph += "subgraph cluster_" + context + dotVar(&snode, context) + " {\n";
  if (!label.empty()) {
    m_graph += "label=\"" + label + "\";\n";
    m_graph += "fontsize=12.0;\n";
  }
  m_graph += snode.DrawNode(context, initiator);
  m_graph += "}\n";
  m_isDirty = true;
}

void Grapher::AddOList(const string& context, const IList& onode, const string& label) {
  m_graph += "subgraph cluster_" + context + dotVar(&onode, context) + " {\n";
  if (!label.empty()) {
    m_graph += "label=\"" + label + "\";\n";
    m_graph += "fontsize=12.0;\n";
  }
  m_graph += onode.DrawNode(context);
  m_graph += "}\n";
  m_isDirty = true;
}

void Grapher::AddIListeners(const string& context, const Connector& connector, const IList& inode) {
  Connector::listener_set ls = connector.GetListeners(inode);
  for (Connector::listener_iter i = ls.begin(); i != ls.end(); ++i) {
    m_graph += dotVar(&inode, context) + " -> " + dotVar(*i, context) + " [constraint=false, weight=0, style=dashed, arrowsize=0.5, color=\"#3333cc\"];\n";
  }
  if (inode.right) {
    AddIListeners(context, connector, *inode.right);
  }
  m_isDirty = true;
}

void Grapher::AddHotlist(const string& context, const Hotlist::hotlist_vec& hotlist) {
  for (Hotlist::hotlist_iter i = hotlist.begin(); i != hotlist.end(); ++i) {
    string color = "#770000";
    switch (i->second) {
    case Hotlist::OP_INSERT:
      color = "#00bb44";
      break;
    case Hotlist::OP_UPDATE:
      color = "#2222cc";
      break;
    case Hotlist::OP_DELETE:
      color = "#cc2222";
      break;
    default:
      throw SError("Cannot add hotlist to Grapher: unknown Hotlist operation");
    }
    m_graph += dotVar(i->first, context) + " [color=\"" + color + "\", penwidth=4.0];\n";
    m_isDirty = true;
  }
}

void Grapher::Signal(const string& context, const void* x, bool isUpdate) {
  string color = "#dd3300";
  if (isUpdate) {
    color = "#003399";
  }
  m_graph += dotVar(x, context) + " [color=\"" + color + "\", penwidth=3.0];\n";
  m_isDirty = true;
}

void Grapher::Save() {
  if (!m_isDirty) {
    g_log.info() << "GGG Grapher: Not saving graph; not dirty";
    return;
  }
  m_graph += "}\n";
  string outfile(m_out_dir + "/" + m_base_filename + Util::pad_str(lexical_cast<string>(m_img_count), '0', 2) + ".dot");
  g_log.debug();
  g_log.debug() << "GGG Grapher: Saving graph " << outfile;
  g_log.debug();
  ofstream out(outfile.c_str());
  if (!out) {
    throw SError("Failed to open output graph file " + outfile);
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
