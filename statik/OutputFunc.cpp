// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "OutputFunc.h"

#include "IList.h"
#include "SLog.h"
#include "STree.h"
#include "State.h"

#include "util/Graphviz.h"
using Util::dotVar;

#include <memory>
#include <string>
#include <vector>
using std::auto_ptr;
using std::string;
using std::vector;

using namespace statik;

auto_ptr<OutputFunc> statik::MakeOutputFunc_Silent() {
  return auto_ptr<OutputFunc>(new OutputFunc_Silent());
}

auto_ptr<OutputFunc> statik::MakeOutputFunc_Basic(const string& name, const string& value) {
  return auto_ptr<OutputFunc>(new OutputFunc_Basic(name, value));
}

auto_ptr<OutputFunc> statik::MakeOutputFunc_IValues(const string& name) {
  return auto_ptr<OutputFunc>(new OutputFunc_IValues(name));
}

auto_ptr<OutputFunc> statik::MakeOutputFunc_Winner() {
  return auto_ptr<OutputFunc>(new OutputFunc_Winner());
}

auto_ptr<OutputFunc> statik::MakeOutputFunc_Sequence() {
  return auto_ptr<OutputFunc>(new OutputFunc_Sequence());
}

auto_ptr<OutputFunc> statik::MakeOutputFunc_Cap(auto_ptr<OutputFunc> outputFunc, const string& cap) {
  return auto_ptr<OutputFunc>(new OutputFunc_Cap(outputFunc, cap));
}

OutputFunc::OutputFunc()
  : m_node(NULL),
    m_ostart(NULL),
    m_oend(NULL) {
}

void OutputFunc::ApproveChild(const STree& child) {
  const Hotlist::hotlist_vec& h = child.GetOutputFunc().GetHotlist();
  g_log.debug() << "**** OutputFunc: " << *m_node << " Approving child " << child << " with hotlist size " << boost::lexical_cast<string>(h.size());
  m_hotlist.Accept(h);
  const emitting_set& e = child.GetOutputFunc().Emitting();
  g_log.debug() << "child emitting " << boost::lexical_cast<string>(e.size()) << ", now we emit " << boost::lexical_cast<string>(m_emitting.size());
  m_emitting.insert(e.begin(), e.end());
}

void OutputFunc::InsertChild(const STree& child) {
  g_log.debug() << "**** OutputFunc: " << *m_node << " Inserting child " << child;
  const emitting_set& e = child.GetOutputFunc().Emitting();
  for (emitting_iter i = e.begin(); i != e.end(); ++i) {
    m_hotlist.Insert(**i);
    m_emitting.insert(*i);
  }
}

void OutputFunc::DeleteChild(const STree& child) {
  g_log.debug() << "**** OutputFunc: " << *m_node << " Deleting child " << child;
  const emitting_set& we = child.GetOutputFunc().WasEmitting();
  for (emitting_iter i = we.begin(); i != we.end(); ++i) {
    m_hotlist.Delete(**i);
    m_emitting.erase(*i);
  }
}

/*
string OutputFunc::DrawEmitting(const string& context) const {
  string s;
  if (m_ostart) {
    s += dotVar(m_ostart->owner, context) + " -> " + dotVar(m_node, context) + " [constraint=false, weight=1, arrowsize=1.2, color=\"#22ee22\"];\n";
  }
  for (emitting_iter i = m_emitting.begin(); i != m_emitting.end(); ++i) {
    s += dotVar((*i)->owner, context) + " -> " + dotVar(m_node, context) + " [constraint=false, weight=1, arrowsize=1.2, color=\"#cc0044\"];\n";
  }
  if (m_oend) {
    s += dotVar(m_ostart->owner, context) + " -> " + dotVar(m_node, context) + " [constraint=false, weight=1, arrowsize=1.2, color=\"#ee2222\"];\n";
  }
  // Also draw the previous connections!
  for (emitting_iter i = m_wasEmitting.begin(); i != m_wasEmitting.end(); ++i) {
    s += dotVar((*i)->owner, context) + " -> " + dotVar(m_node, context) + " [constraint=false, weight=1, arrowsize=1.2, color=\"#6600ee\"];\n";
  }
  return s;
}
*/

/* OutputFunc_Basic */

OutputFunc_Basic::OutputFunc_Basic(const string& name, const string& value)
  : OutputFunc(),
    m_onode(NULL),
    m_name(name),
    m_value(value) {
}

void OutputFunc_Basic::Cleanup() {
  this->OutputFunc::Cleanup();
  if (m_onode) {
    m_node->GetConnector().UnlinkONode(*m_onode);
  }
}

void OutputFunc_Basic::operator() () {
  m_wasEmitting = m_emitting;
  m_hotlist.Clear();
  if (!m_onode) {
    m_onode = m_node->GetConnector().InsertONode(auto_ptr<IList>(new IList(m_name, m_value)));
    m_ostart = m_onode;
    m_oend = m_onode;
    m_emitting.insert(m_onode);
    m_hotlist.Insert(*m_onode);
  }
  g_log.debug() << "Basic output func now has hotlist size " << boost::lexical_cast<string>(m_hotlist.Size());
  g_log.debug() << m_hotlist.Print();
}

auto_ptr<OutputFunc> OutputFunc_Basic::Clone() {
  return auto_ptr<OutputFunc>(new OutputFunc_Basic(m_name, m_value));
}

/* OutputFunc_IValues */

OutputFunc_IValues::OutputFunc_IValues(const string& name)
  : OutputFunc(),
    m_onode(NULL),
    m_name(name) {
}

void OutputFunc_IValues::Cleanup() {
  this->OutputFunc::Cleanup();
  if (m_onode) {
    m_node->GetConnector().UnlinkONode(*m_onode);
  }
}

void OutputFunc_IValues::operator() () {
  m_wasEmitting = m_emitting;
  m_hotlist.Clear();
  bool isFirstTime = false;
  if (!m_onode) {
    isFirstTime = true;
    m_onode = m_node->GetConnector().InsertONode(auto_ptr<IList>(new IList(m_name)));
    m_ostart = m_onode;
    m_oend = m_onode;
    m_emitting.insert(m_onode);
    m_hotlist.Insert(*m_onode);
  }

  string value;
  const IList* ilast = NULL;
  if (m_node->GetState().IsComplete()) {
    ilast = &m_node->IEnd();
  }
  for (const IList* i = &m_node->IStart(); i != ilast; i = i->right) {
    value += i->value;
  }
  if (value != m_onode->value) {
    m_onode->value = value;
    if (!isFirstTime) {
      m_hotlist.Update(*m_onode);
    }
  }
  g_log.debug() << "IValues output func now has hotlist size " << boost::lexical_cast<string>(m_hotlist.Size());
  g_log.debug() << m_hotlist.Print();
}

auto_ptr<OutputFunc> OutputFunc_IValues::Clone() {
  return auto_ptr<OutputFunc>(new OutputFunc_IValues(m_name));
}

/* OutputFunc_Winner */

OutputFunc_Winner::OutputFunc_Winner()
  : OutputFunc(),
    m_winner(NULL) {
}

void OutputFunc_Winner::operator() () {
  m_wasEmitting = m_emitting;
  m_emitting.clear();
  m_hotlist.Clear();
  m_ostart = NULL;
  m_oend = NULL;

  const State& state = m_node->GetState();
  if (!state.IsEmitting()) {
    g_log.debug() << "OutputFunc_Winner " << *m_node << " is not emitting; skipping update";
    return;
  }
  bool isComplete = state.IsComplete();
  const STree* winner = NULL;
  for (STree::child_iter i = m_node->children.begin(); i != m_node->children.end(); ++i) {
    const State& istate = i->GetState();
    if (istate.IsBad() || istate.IsOK()) {
      continue;
    }
    if ((isComplete && istate.IsComplete()) || (!isComplete && istate.IsDone())) {
      if (!state.IsLocked() || istate.IsLocked()) {
        winner = &*i;
        break;
      }
    }
  }
  if (!winner) {
    throw SError("OutputFunc_Winner for " + string(*m_node) + " failed to find winner");
  }
  g_log.debug() << "**** OutputFunc_Winner: " << *m_node << " Declaring winner " << *winner;
  m_ostart = winner->GetOutputFunc().OStart();
  m_oend = winner->GetOutputFunc().OEnd();
  if (winner == m_winner) {
    ApproveChild(*winner);
  } else {
    if (m_winner) {
      g_log.debug() << "OutputFunc_Winner: un-winning (deleting) old winner " << *m_winner;
      DeleteChild(*m_winner);
    }
    InsertChild(*winner);
    m_winner = winner;
  }

  g_log.debug() << "OutputFunc_Winner " << *m_node << " done update; hotlist has size " << boost::lexical_cast<string>(m_hotlist.Size());
  g_log.debug() << m_hotlist.Print();
}

auto_ptr<OutputFunc> OutputFunc_Winner::Clone() {
  return auto_ptr<OutputFunc>(new OutputFunc_Winner());
}

/* OutputFunc_Sequence */

void OutputFunc_Sequence::operator() () {
  m_wasEmitting = m_emitting;
  m_emitting.clear();
  m_hotlist.Clear();
  m_ostart = NULL;
  m_oend = NULL;

  g_log.debug() << "OutputFunc_Sequence() " << *m_node << "; previously emit children size=" << m_emitChildren.size();
  const State& state = m_node->GetState();
  if (!state.IsEmitting()) {
    return;
  }
  emitchildren_set wereEmit = m_emitChildren;
  m_emitChildren.clear();
  for (STree::child_iter i = m_node->children.begin(); i != m_node->children.end(); ++i) {
    g_log.debug() << "Considering child " << *i;
    const State& istate = i->GetState();
    if (!istate.IsEmitting()) {
      // Delete children that are no longer being emit
      g_log.debug() << "- it's not emitting; time to delete";
      for (emitchildren_iter we = wereEmit.begin(); we != wereEmit.end(); ++we) {
        g_log.debug() << "Deleting previously-emit child " << **we << " from " << *i;
        DeleteChild(**we);
      }
      // End the sequence at the first non-emit child
      break;
    }
    if (wereEmit.end() == wereEmit.find(&*i)) {
      InsertChild(*i);
    } else {
      ApproveChild(*i);
      wereEmit.erase(&*i);
    }
    OutputFunc& iof = i->GetOutputFunc();
    if ((iof.OStart() && !iof.OEnd()) || (!iof.OStart() && iof.OEnd())) {
      throw SError("OutputFunc_Sequence() found " + string(*i) + " with only an ostart or oend, but not both");
    }
    m_emitChildren.insert(&*i);
    if (!iof.OStart()) {
      continue;
    }
    if (!m_ostart) {
      m_ostart = iof.OStart();
    } else {
      iof.OStart()->left = m_oend;
      m_oend->right = iof.OStart();
    }
    m_oend = iof.OEnd();
  }

  g_log.debug() << "OutputFunc_Sequence " << *m_node << " done update; hotlist has size " << boost::lexical_cast<string>(m_hotlist.Size());
  g_log.debug() << m_hotlist.Print();
}

auto_ptr<OutputFunc> OutputFunc_Sequence::Clone() {
  return auto_ptr<OutputFunc>(new OutputFunc_Sequence());
}

/* OutputFunc_Cap */

OutputFunc_Cap::OutputFunc_Cap(auto_ptr<OutputFunc> outputFunc, const string& cap)
  : OutputFunc(),
    m_outputFunc(outputFunc),
    m_cap(cap),
    m_capStart(NULL),
    m_capEnd(NULL) {
}

void OutputFunc_Cap::Init(const STree& x) {
  m_node = &x;
  m_outputFunc->Init(x);
}

void OutputFunc_Cap::Cleanup() {
  this->OutputFunc::Cleanup();
  if (m_capStart) {
    m_node->GetConnector().UnlinkONode(*m_capStart);
  }
  if (m_capEnd) {
    m_node->GetConnector().UnlinkONode(*m_capEnd);
  }
}

void OutputFunc_Cap::operator() () {
  m_wasEmitting = m_emitting;
  m_emitting.clear();
  m_hotlist.Clear();
  if (!m_capStart) {
    m_capStart = m_node->GetConnector().InsertONode(auto_ptr<IList>(new IList(m_cap)));
    m_capEnd = m_node->GetConnector().InsertONode(auto_ptr<IList>(new IList("/" + m_cap)));
    m_ostart = m_capStart;
    m_oend = m_capEnd;
    m_capStart->right = m_capEnd;
    m_capEnd->left = m_capStart;
    m_hotlist.Insert(*m_capStart);
    m_hotlist.Insert(*m_capEnd);
  }
  m_emitting.insert(m_capStart);
  m_emitting.insert(m_capEnd);

  (*m_outputFunc)();
  if (m_outputFunc->OStart()) {
    m_capStart->right = m_outputFunc->OStart();
    m_outputFunc->OStart()->left = m_capStart;
  }
  if (m_outputFunc->OEnd()) {
    m_outputFunc->OEnd()->right = m_capEnd;
    m_capEnd->left = m_outputFunc->OEnd();
  }
  m_emitting.insert(m_outputFunc->Emitting().begin(), m_outputFunc->Emitting().end());
  m_hotlist.Accept(m_outputFunc->GetHotlist());

  g_log.debug() << "OutputFunc_Cap " << *m_node << " done update; hotlist has size " << boost::lexical_cast<string>(m_hotlist.Size());
  g_log.debug() << m_hotlist.Print();
}

auto_ptr<OutputFunc> OutputFunc_Cap::Clone() {
  return auto_ptr<OutputFunc>(new OutputFunc_Cap(m_outputFunc->Clone(), m_cap));
}
