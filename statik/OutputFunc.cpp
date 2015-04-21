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
#include <utility>
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

/* OutputFunc_Basic */

OutputFunc_Basic::OutputFunc_Basic(const string& name, const string& value)
  : OutputFunc(),
    m_onode(name, value) {
}

void OutputFunc_Basic::operator() () {
  g_log.debug() << "OutputFunc_Basic " << m_onode;
  if (!m_ostart) {
    g_log.debug() << " - init";
    m_ostart = &m_onode;
    m_oend = &m_onode;
    m_state.onodes.insert(&m_onode);
  }
}

auto_ptr<OutputFunc> OutputFunc_Basic::Clone() {
  return auto_ptr<OutputFunc>(new OutputFunc_Basic(m_onode.name, m_onode.value));
}

/* OutputFunc_IValues */

OutputFunc_IValues::OutputFunc_IValues(const string& name)
  : OutputFunc(),
    m_onode(name) {
}

void OutputFunc_IValues::operator() () {
  g_log.debug() << "OutputFunc_IValues " << m_onode;
  if (!m_ostart) {
    g_log.debug() << " - init";
    m_ostart = &m_onode;
    m_oend = &m_onode;
    m_state.onodes.insert(&m_onode);
  }

  string value;
  const IList* ilast = NULL;
  if (m_node->GetState().IsComplete()) {
    ilast = &m_node->IEnd();
  }
  for (const IList* i = &m_node->IStart(); i != ilast; i = i->right) {
    value += i->value;
  }
  m_state.value = value;
}

auto_ptr<OutputFunc> OutputFunc_IValues::Clone() {
  return auto_ptr<OutputFunc>(new OutputFunc_IValues(m_onode.name));
}

/* OutputFunc_Winner */

OutputFunc_Winner::OutputFunc_Winner()
  : OutputFunc(),
    m_winner(NULL) {
}

void OutputFunc_Winner::operator() () {
  g_log.debug() << "OutputFunc_Winner() " << *m_node;
  m_state.children.clear();
  const State& state = m_node->GetState();
  const STree* winner = NULL;
  for (STree::child_iter i = m_node->children.begin(); i != m_node->children.end(); ++i) {
    const State& istate = (*i)->GetState();
    if (istate.IsBad() || istate.IsOK()) {
      continue;
    }
    bool isComplete = state.IsComplete();
    if ((isComplete && istate.IsComplete()) || (!isComplete && istate.IsDone())) {
      if (!state.IsLocked() || istate.IsLocked()) {
        winner = *i;
        break;
      }
    }
  }
  if (!winner && m_winner) {
    g_log.info() << "**** OutputFunc_Winner: Did not find a winner, so just taking last winner " << *m_winner;
    winner = m_winner;
  }
  if (winner) {
    g_log.debug() << "**** OutputFunc_Winner: " << *m_node << " Declaring winner " << *winner;
    m_winner = winner;
    m_state.children.insert(m_winner);
  } else {
    g_log.debug() << "**** OutputFunc_Winner: No winner.";
  }
}

void OutputFunc_Winner::ConnectONodes() {
  g_log.debug() << "OutputFunc_Winner::ConnectONodes() " << *m_node;
  if (m_winner) {
    g_log.debug() << "**** OutputFunc_Winner: " << *m_node << " Setting ONodes from winner " << *m_winner;
    m_ostart = m_winner->GetOutputFunc().OStart();
    m_oend = m_winner->GetOutputFunc().OEnd();
    if (!m_ostart) {
      g_log.error() << "Failed to set OStart from winner";
    }
  } else {
    m_ostart = NULL;
    m_oend = NULL;
  }
}

auto_ptr<OutputFunc> OutputFunc_Winner::Clone() {
  return auto_ptr<OutputFunc>(new OutputFunc_Winner());
}

/* OutputFunc_Sequence */

void OutputFunc_Sequence::operator() () {
  g_log.debug() << "OutputFunc_Sequence() " << *m_node;
  m_ostart = NULL;
  m_oend = NULL;
  m_state.children.clear();

  if (m_node->GetState().IsBad()) {
    m_state.children.insert(m_node->children.begin(), m_node->children.end());
    return;
  }

  for (STree::child_iter child = m_node->children.begin(); child != m_node->children.end(); ++child) {
    g_log.debug() << " - Considering child " << **child;

    const State& istate = (*child)->GetState();
    if (istate.IsPending()) {
      throw SError("OutputFunc Sequence found Pending child");
    } else if (istate.IsBad()) {
      g_log.debug() << "**** OutputFunc_Sequence: " << *m_node << " aborting at bad child " << **child;
      break;
    } else if (istate.IsDone() || istate.IsOK()) {
      g_log.debug() << "**** OutputFunc_Sequence: " << *m_node << " aborting after " << (istate.IsOK() ? "ok" : "done") << " child " << **child;
      m_state.children.insert(*child);
      break;
    } else if (!istate.IsComplete()) {
      throw SError("OutputFunc Seq found child in unknown state");
    }
    m_state.children.insert(*child);
  }
}

void OutputFunc_Sequence::ConnectONodes() {
  g_log.debug() << "OutputFunc_Sequence::ConnectONodes() " << *m_node;
  g_log.debug() << "OutputFunc_Sequence at " << *m_node << ": Connecting ONodes";
  for (STree::child_iter child = m_node->children.begin(); child != m_node->children.end(); ++child) {
    g_log.debug() << " - processing child: " << **child;
    // If m_node is Complete and this is the first child past the last approved
    // child, then discard the output of this child onwards.
    const State& istate = (*child)->GetState();
    if (istate.IsBad()) {
      g_log.debug() << " - child is bad -- abort";
      break;
    }

    OutputFunc& cof = (*child)->GetOutputFunc();
    if ((cof.OStart() && !cof.OEnd()) || (!cof.OStart() && cof.OEnd())) {
      throw SError("OutputFunc_Sequence() found " + string(**child) + " with only an ostart or oend, but not both");
    }
    if (!cof.OStart()) {
      g_log.debug() << " - child has no OStart; skipping";
      continue;
    }
    if (!m_ostart) {
      m_ostart = cof.OStart();
      m_ostart->left = NULL;
    } else {
      cof.OStart()->left = m_oend;
      m_oend->right = cof.OStart();
    }
    m_oend = cof.OEnd();
    m_oend->right = NULL;
    if (!istate.IsComplete()) {
      g_log.debug() << " - child is not complete; aborting";
      break;
    }
  }
}

auto_ptr<OutputFunc> OutputFunc_Sequence::Clone() {
  return auto_ptr<OutputFunc>(new OutputFunc_Sequence());
}

/* OutputFunc_Cap */

OutputFunc_Cap::OutputFunc_Cap(auto_ptr<OutputFunc> outputFunc, const string& cap)
  : OutputFunc(),
    m_outputFunc(outputFunc),
    m_cap(cap),
    m_capStart("(" + m_cap),
    m_capEnd(")") {
}

void OutputFunc_Cap::Init(const STree& x) {
  m_node = &x;
  m_outputFunc->Init(x);
}

void OutputFunc_Cap::operator() () {
  g_log.debug() << "OutputFunc_Cap() " << *m_node;
  if (!m_ostart) {
    m_ostart = &m_capStart;
    m_oend = &m_capEnd;
  }

  (*m_outputFunc)();
  m_state = m_outputFunc->GetState();
  m_state.onodes.insert(&m_capStart);
  m_state.onodes.insert(&m_capEnd);
}

void OutputFunc_Cap::ConnectONodes() {
  g_log.debug() << "OutputFunc_Cap at " << *m_node << ": Connecting ONodes";
  m_outputFunc->ConnectONodes();
  if (m_outputFunc->OStart()) {
    m_capStart.right = m_outputFunc->OStart();
    m_outputFunc->OStart()->left = &m_capStart;
  } else {
    m_capStart.right = &m_capEnd;
  }
  if (m_outputFunc->OEnd()) {
    m_outputFunc->OEnd()->right = &m_capEnd;
    m_capEnd.left = m_outputFunc->OEnd();
  } else {
    m_capEnd.left = &m_capStart;
  }
}

auto_ptr<OutputFunc> OutputFunc_Cap::Clone() {
  return auto_ptr<OutputFunc>(new OutputFunc_Cap(m_outputFunc->Clone(), m_cap));
}
