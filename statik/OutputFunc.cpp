// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "OutputFunc.h"

#include "List.h"
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

auto_ptr<OutputFunc> statik::MakeOutputFunc_Pass() {
  return auto_ptr<OutputFunc>(new OutputFunc_Pass());
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

void OutputFunc::Sync() {
  if (m_output.empty()) {
    m_ostart = NULL;
    m_oend = NULL;
    return;
  }
  {
    OutputItem& front = m_output.front();
    if (front.onode) {
      m_ostart = front.onode;
    } else {
      m_ostart = front.child->GetOutputFunc().OStart();
    }
  } {
    OutputItem& back = m_output.back();
    if (back.onode) {
      m_oend = back.onode;
    } else {
      m_oend = back.child->GetOutputFunc().OEnd();
    }
  }
}

/* OutputFunc_Silent */

auto_ptr<OutputFunc> OutputFunc_Silent::Clone() {
  return auto_ptr<OutputFunc>(new OutputFunc_Silent());
}

/* OutputFunc_Pass */

auto_ptr<OutputFunc> OutputFunc_Pass::Clone() {
  return std::auto_ptr<OutputFunc>(new OutputFunc_Pass());
}

void OutputFunc_Pass::operator() () {
  if (m_node->children.empty()) {
    // Leave our dangling old state, I guess
  } else if (m_node->children.size() != 1) {
    throw SError("OutputFunc_Pass: must have exactly 1 child");
  }
  const STree& child = *m_node->children.front();
  m_output.clear();
  m_output.push_back(OutputItem(child, m_node->GetState().GetStation()));
}

/* OutputFunc_Basic */

OutputFunc_Basic::OutputFunc_Basic(const string& name, const string& value)
  : OutputFunc(),
    m_onode(name, value) {
}

void OutputFunc_Basic::operator() () {
  g_log.debug() << "OutputFunc_Basic " << m_onode;
  if (m_output.empty()) {
    m_output.push_back(OutputItem(m_onode, m_node->GetState().GetStation()));
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
  if (m_output.empty()) {
    m_output.push_back(OutputItem(m_onode, m_node->GetState().GetStation()));
  }
  /*
  string value;
  const List* ilast = NULL;
  if (m_node->GetState().IsComplete()) {
    ilast = &m_node->IEnd();
  }
  for (const List* i = &m_node->IStart(); i != ilast; i = i->right) {
    value += i->value;
  }
  m_state.value = value;
  */
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
  m_output.clear();
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
    m_output.push_back(OutputItem(*m_winner, m_node->GetState().GetStation()));
  } else {
    g_log.debug() << "**** OutputFunc_Winner: No winner.";
  }
}

auto_ptr<OutputFunc> OutputFunc_Winner::Clone() {
  return auto_ptr<OutputFunc>(new OutputFunc_Winner());
}

/* OutputFunc_Sequence */

void OutputFunc_Sequence::operator() () {
  g_log.debug() << "OutputFunc_Sequence() " << *m_node;
  m_output.clear();

  if (m_node->GetState().IsBad()) {
    for (STree::child_iter child = m_node->children.begin(); child != m_node->children.end(); ++child) {
      m_output.push_back(OutputItem(**child, m_node->GetState().GetStation()));
    }
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
      m_output.push_back(OutputItem(**child, m_node->GetState().GetStation()));
      break;
    } else if (!istate.IsComplete()) {
      throw SError("OutputFunc Seq found child in unknown state");
    }
    m_output.push_back(OutputItem(**child, m_node->GetState().GetStation()));
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
  (*m_outputFunc)();
  m_output = m_outputFunc->GetOutput();
}

auto_ptr<OutputFunc> OutputFunc_Cap::Clone() {
  return auto_ptr<OutputFunc>(new OutputFunc_Cap(m_outputFunc->Clone(), m_cap));
}
