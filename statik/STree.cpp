// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "STree.h"

#include "Connector.h"
#include "Rule.h"
#include "SLog.h"

#include "util/Graphviz.h"
using Util::dotVar;

#include <memory>
#include <ostream>
#include <string>
using std::auto_ptr;
using std::ostream;
using std::string;

using namespace statik;

/* public */

STree::STree(Connector& connector,
               const Rule& rule, STree* parent,
               auto_ptr<RestartFunc> restartFunc,
               auto_ptr<ComputeFunc> computeFunc,
               auto_ptr<OutputFunc> outputFunc)
  : m_connector(connector),
    m_rule(rule),
    m_parent(parent),
    m_isClear(true),
    depth(m_parent ? m_parent->depth + 1 : 0),
    m_restartFunc(restartFunc),
    m_computeFunc(computeFunc),
    m_outputFunc(outputFunc) {
  m_restartFunc->Init(*this);
  m_computeFunc->Init(*this);
  m_outputFunc->Init(*this);
}

void STree::RestartNode(const IList& istart) {
  g_log.info() << "Restarting node " << *this << " with inode " << istart;
  m_isClear = false;
  m_iconnection.Restart(istart);
  m_connector.DrawGraph(*this, &istart);
  g_log.info() << " - calling restart func";
  (*m_restartFunc.get())(istart);
  // We don't enqueue the compute here.  Let the node decide when to do that.
}

void STree::ComputeNode(ConnectorAction::Action action, const IList& inode, const STree* initiator) {
  if (m_isClear) {
    throw SError("Cannot compute node " + string(*this) + " which has been cleared");
  }
  g_log.info() << "Computing node " << *this << " with inode " << inode << " and initiator " << (initiator ? string(*initiator) : "<null>");
  const State old_state = m_state;
  const IList& old_iend = IEnd();
  size_t old_size = ISize();
  (*m_computeFunc)(action, inode, initiator);
  m_connector.DrawGraph(*this);

  bool hasChanged1 = &old_iend != &IEnd();
  bool hasChanged2 = old_state != m_state;
  bool hasChanged = hasChanged1 || hasChanged2;
  g_log.info() << " - - - - " << *this << " has " << (hasChanged ? "" : "NOT ") << "changed  " << hasChanged1 << ":" << hasChanged2;
  if (hasChanged) {
    m_connector.TouchNode(*this);
    if (m_parent) {
      size_t new_size = ISize();
      ConnectorAction::Action a;
      if (old_size < new_size) {
        a = ConnectorAction::ChildGrow;
      } else if (old_size == new_size) {
        a = ConnectorAction::ChildUpdate;
      } else {
        a = ConnectorAction::ChildShrink;
      }
      m_connector.Enqueue(ConnectorAction(a, *m_parent, inode, this));
    }
  }
}

void STree::ClearNode() {
  g_log.info() << "Clearing node " << *this;
  if (m_isClear) {
    g_log.info() << " - already clear!";
    return;
  }
  for (child_mod_iter i = children.begin(); i != children.end(); ++i) {
    (*i)->ClearNode();
  }
  children.clear();
  m_state.Clear();
  m_connector.ClearNode(*this);
  m_isClear = true;
}

bool STree::ContainsINode(const IList& inode) const {
  for (const IList* i = &m_iconnection.Start(); i != NULL && i != &m_iconnection.End(); i = i->right) {
    if (&inode == i) {
      return true;
    }
  }
  return false;
}

STree::operator std::string() const {
  return m_rule.Name() + ":" + string(GetState());
}

string STree::DrawNode(const string& context) const {
  string s;
  // Style the node to indicate its State
  string fillcolor = "#000000";
  if (m_state.IsInit()) {
    fillcolor = "#eeeeee";
  } else if (m_state.IsBad()) {
    fillcolor = "#ff8888";
  } else if (m_state.IsOK()) {
    // default
    fillcolor = "#ffeebb";
  } else if (m_state.IsDone()) {
    fillcolor = "#99ffcc";
  } else if (m_state.IsComplete()) {
    fillcolor = "#aaaaff";
  } else {
    throw SError("Stree::DrawNode() - unknown state");
  }
  s += dotVar(this, context) + " [label=\"" + Util::safeLabelStr(m_rule.Name()) + "\", style=\"filled\", fillcolor=\"" + fillcolor + "\", fontsize=12.0];\n";

  // Connect the node to its IConnection
  if (!m_isClear) {
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
  }

  // Add its child connections, and draw the children
  // Make sure the children will be ordered in the output graph
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    s += dotVar(this, context) + " -> " + dotVar(*i, context) + ";\n";
  }
  if (children.size() > 0) {
    s += "{ rank=same;\n";
    child_iter prev_child = children.begin();
    for (child_iter i = children.begin() + 1; i != children.end(); ++i) {
      s += dotVar(*prev_child, context) + " -> " + dotVar(*i, context) + " [style=\"invis\"];\n";
      prev_child = i;
    }
    s += "}\n";
  }
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    s += (*i)->DrawNode(context);
  }
  //s += m_outputFunc->DrawEmitting(context, *this);
  return s;
}

/* non-member */

ostream& statik::operator<< (ostream& out, const STree& node) {
  out << string(node);
  return out;
}
