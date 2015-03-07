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
  m_hotlist.Clear();
  if (!m_ostart) {
    m_ostart = &m_onode;
    m_oend = &m_onode;
    m_emitting.insert(&m_onode);
    m_hotlist.Insert(m_onode);
  }
  g_log.debug() << "Basic output func now has hotlist size " << m_hotlist.Size();
  g_log.debug() << m_hotlist.Print();
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
  m_hotlist.Clear();
  bool isFirstTime = false;
  if (!m_ostart) {
    isFirstTime = true;
    m_ostart = &m_onode;
    m_oend = &m_onode;
    m_emitting.insert(&m_onode);
    m_hotlist.Insert(m_onode);
  }

  string value;
  const IList* ilast = NULL;
  if (m_node->GetState().IsComplete()) {
    ilast = &m_node->IEnd();
  }
  for (const IList* i = &m_node->IStart(); i != ilast; i = i->right) {
    value += i->value;
  }
  if (value != m_onode.value) {
    m_onode.value = value;
    if (!isFirstTime) {
      m_hotlist.Update(m_onode);
    }
  }
  g_log.debug() << "IValues output func now has hotlist size " << m_hotlist.Size();
  g_log.debug() << m_hotlist.Print();
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
  m_wasEmitting = m_emitting;
  m_emitting.clear();
  m_hotlist.Clear();
  m_ostart = NULL;
  m_oend = NULL;

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
    m_ostart = winner->GetOutputFunc().OStart();
    m_oend = winner->GetOutputFunc().OEnd();
    const emitting_set& e = winner->GetOutputFunc().Emitting();
    m_emitting.insert(e.begin(), e.end());
    if (winner == m_winner) {
      g_log.debug() << "**** OutputFunc_Winner: " << *m_node << " Approving repeat-winner " << *winner;
      m_hotlist.Accept(winner->GetOutputFunc().GetHotlist());
    } else {
      if (m_winner) {
        g_log.debug() << "OutputFunc_Winner: un-winning (deleting) old winner " << *m_winner;
        for (emitting_iter i = m_wasEmitting.begin(); i != m_wasEmitting.end(); ++i) {
          m_hotlist.Delete(**i);
        }
      }
      g_log.debug() << "**** OutputFunc_Winner: " << *m_node << " Inserting new winner " << *winner;
      for (emitting_iter i = e.begin(); i != e.end(); ++i) {
        m_hotlist.Insert(**i);
      }
      m_winner = winner;
    }

    // We stole the winner's output hotlist
    winner->GetOutputFunc().ClearHotlist();

    g_log.debug() << "winner is emitting " << e.size() << ", now we emit " << m_emitting.size();
  } else {
    g_log.debug() << "**** OutputFunc_Winner: No winner.";
  }

  g_log.debug() << "OutputFunc_Winner " << *m_node << " done update; hotlist has size " << m_hotlist.Size();
  g_log.debug() << m_hotlist.Print();
}

auto_ptr<OutputFunc> OutputFunc_Winner::Clone() {
  return auto_ptr<OutputFunc>(new OutputFunc_Winner());
}

/* OutputFunc_Sequence */

void OutputFunc_Sequence::operator() () {
  m_emitting.clear();
  m_hotlist.Clear();
  m_ostart = NULL;
  m_oend = NULL;

  emitbychild_map wereEmitByChild = m_emitByChild;
  m_emitByChild.clear();
  g_log.debug() << "OutputFunc_Sequence() " << *m_node << "; previously emit children size=" << wereEmitByChild.size();
  for (STree::child_iter child = m_node->children.begin(); child != m_node->children.end(); ++child) {
    g_log.debug() << "Considering child " << **child;

    // If m_node is Complete and this is the first child past the last approved
    // child, then discard the output of this child onwards.
    const State& istate = (*child)->GetState();
    if (istate.IsComplete() && &(*child)->IStart() == &m_node->IEnd()) {
      g_log.debug() << "**** OutputFunc_Sequence: " << *m_node << " past complete, so blocking at child " << **child;
      break;
    }

    const emitting_set& e = (*child)->GetOutputFunc().Emitting();
    m_emitting.insert(e.begin(), e.end());
    emitbychild_mod_iter emit_i = wereEmitByChild.find(*child);
    if (wereEmitByChild.end() == emit_i) {
      g_log.debug() << "**** OutputFunc_Sequence: " << *m_node << " Inserting child " << **child;
      for (emitting_iter i = e.begin(); i != e.end(); ++i) {
        m_hotlist.Insert(**i);
      }
    } else {
      g_log.debug() << "**** OutputFunc_Sequence: " << *m_node << " Approving child " << **child;
      m_hotlist.Accept((*child)->GetOutputFunc().GetHotlist());
      wereEmitByChild.erase(emit_i);
    }
    m_emitByChild.insert(std::make_pair(*child, e));

    OutputFunc& cof = (*child)->GetOutputFunc();
    if ((cof.OStart() && !cof.OEnd()) || (!cof.OStart() && cof.OEnd())) {
      throw SError("OutputFunc_Sequence() found " + string(**child) + " with only an ostart or oend, but not both");
    }
    if (!cof.OStart()) {
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

    // We stole the child's output hotlist
    (*child)->GetOutputFunc().ClearHotlist();
  }

  // Remove hotlist items that came from children that are no longer being emit
  for (emitbychild_iter i = wereEmitByChild.begin(); i != wereEmitByChild.end(); ++i) {
    for (emitting_iter j = i->second.begin(); j != i->second.end(); ++j) {
      m_hotlist.Delete(**j);
    }
  }

  g_log.debug() << "OutputFunc_Sequence " << *m_node << " done update; hotlist has size " << m_hotlist.Size();
  g_log.debug() << " - OStart: " << (m_ostart ? string(*m_ostart) : "<null>") << ", OEnd: " << (m_oend ? string(*m_oend) : "<null>");
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
    m_capStart("(" + m_cap),
    m_capEnd(")") {
}

void OutputFunc_Cap::Init(const STree& x) {
  m_node = &x;
  m_outputFunc->Init(x);
}

void OutputFunc_Cap::operator() () {
  m_emitting.clear();
  m_hotlist.Clear();
  if (!m_ostart) {
    m_ostart = &m_capStart;
    m_oend = &m_capEnd;
    m_capStart.right = &m_capEnd;
    m_capEnd.left = &m_capStart;
    m_hotlist.Insert(m_capStart);
    m_hotlist.Insert(m_capEnd);
  }
  m_emitting.insert(&m_capStart);
  m_emitting.insert(&m_capEnd);

  (*m_outputFunc)();
  if (m_outputFunc->OStart()) {
    m_capStart.right = m_outputFunc->OStart();
    m_outputFunc->OStart()->left = &m_capStart;
  }
  if (m_outputFunc->OEnd()) {
    m_outputFunc->OEnd()->right = &m_capEnd;
    m_capEnd.left = m_outputFunc->OEnd();
  }
  m_emitting.insert(m_outputFunc->Emitting().begin(), m_outputFunc->Emitting().end());
  m_hotlist.Accept(m_outputFunc->GetHotlist());
  m_outputFunc->ClearHotlist();

  g_log.debug() << "OutputFunc_Cap " << *m_node << " done update; hotlist has size " << m_hotlist.Size();
  g_log.debug() << m_hotlist.Print();
}

auto_ptr<OutputFunc> OutputFunc_Cap::Clone() {
  return auto_ptr<OutputFunc>(new OutputFunc_Cap(m_outputFunc->Clone(), m_cap));
}
