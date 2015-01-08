// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "OutputFunc.h"

#include "FWTree.h"
#include "IList.h"
#include "State.h"

#include "util/Graphviz.h"
using Util::dotVar;

#include <memory>
#include <string>
#include <vector>
using std::auto_ptr;
using std::string;
using std::vector;

using namespace fw;

auto_ptr<OutputFunc> fw::MakeOutputFunc_Silent(Log& log) {
  return auto_ptr<OutputFunc>(new OutputFunc_Silent(log));
}

auto_ptr<OutputFunc> fw::MakeOutputFunc_Basic(Log& log, const string& name, const string& value) {
  return auto_ptr<OutputFunc>(new OutputFunc_Basic(log, name, value));
}

auto_ptr<OutputFunc> fw::MakeOutputFunc_IValues(Log& log, const string& name) {
  return auto_ptr<OutputFunc>(new OutputFunc_IValues(log, name));
}

auto_ptr<OutputFunc> fw::MakeOutputFunc_Winner(Log& log) {
  return auto_ptr<OutputFunc>(new OutputFunc_Winner(log));
}

auto_ptr<OutputFunc> fw::MakeOutputFunc_Sequence(Log& log) {
  return auto_ptr<OutputFunc>(new OutputFunc_Sequence(log));
}

auto_ptr<OutputFunc> fw::MakeOutputFunc_Cap(Log& log, auto_ptr<OutputFunc> outputFunc, const string& cap) {
  return auto_ptr<OutputFunc>(new OutputFunc_Cap(log, outputFunc, cap));
}

OutputFunc::OutputFunc(Log& log)
  : m_log(log),
    m_node(NULL),
    m_ostart(NULL),
    m_oend(NULL) {
}

void OutputFunc::ApproveChild(const FWTree& child) {
  const Hotlist::hotlist_vec& h = child.GetOutputFunc().GetHotlist();
  m_log.debug("**** OutputFunc: " + string(*m_node) + " Approving child " + string(child) + " with hotlist size " + boost::lexical_cast<string>(h.size()));
  m_hotlist.Accept(h);
  const emitting_set& e = child.GetOutputFunc().Emitting();
  m_log.debug("child emitting " + boost::lexical_cast<string>(e.size()) + ", now we emit " + boost::lexical_cast<string>(m_emitting.size()));
  m_emitting.insert(e.begin(), e.end());
}

void OutputFunc::InsertChild(const FWTree& child) {
  m_log.debug("**** OutputFunc: " + string(*m_node) + " Inserting child " + string(child));
  const emitting_set& e = child.GetOutputFunc().Emitting();
  for (emitting_iter i = e.begin(); i != e.end(); ++i) {
    m_hotlist.Insert(**i);
    m_emitting.insert(*i);
  }
}

void OutputFunc::DeleteChild(const FWTree& child) {
  m_log.debug("**** OutputFunc: " + string(*m_node) + " Deleting child " + string(child));
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

OutputFunc_Basic::OutputFunc_Basic(Log& log, const string& name, const string& value)
  : OutputFunc(log),
    m_onode(name, value),
    m_isFirstTime(true) {
}

void OutputFunc_Basic::operator() () {
  m_wasEmitting = m_emitting;
  m_hotlist.Clear();
  if (m_isFirstTime) {
    m_ostart = &m_onode;
    m_oend = &m_onode;
    m_emitting.insert(&m_onode);
    m_hotlist.Insert(m_onode);
    m_isFirstTime = false;
  }
  m_log.debug("Basic output func now has hotlist size " + boost::lexical_cast<string>(m_hotlist.Size()));
  m_log.debug(m_hotlist.Print());
}

/* OutputFunc_IValues */

OutputFunc_IValues::OutputFunc_IValues(Log& log, const string& name)
  : OutputFunc(log),
    m_onode(name),
    m_isFirstTime(true) {
}

void OutputFunc_IValues::operator() () {
  m_wasEmitting = m_emitting;
  m_hotlist.Clear();
  if (m_isFirstTime) {
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
    if (!m_isFirstTime) {
      m_hotlist.Update(m_onode);
    }
  }
  m_isFirstTime = false;
  m_log.debug("IValues output func now has hotlist size " + boost::lexical_cast<string>(m_hotlist.Size()));
  m_log.debug(m_hotlist.Print());
}

/* OutputFunc_Winner */

OutputFunc_Winner::OutputFunc_Winner(Log& log)
  : OutputFunc(log),
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
    m_log.debug("OutputFunc_Winner " + string(*m_node) + " is not emitting; skipping update");
    return;
  }
  bool isComplete = state.IsComplete();
  const FWTree* winner = NULL;
  for (FWTree::child_iter i = m_node->children.begin(); i != m_node->children.end(); ++i) {
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
    throw FWError("OutputFunc_Winner for " + string(*m_node) + " failed to find winner");
  }
  m_log.debug("**** OutputFunc_Winner: " + string(*m_node) + " Declaring winner " + string(*winner));
  m_ostart = winner->GetOutputFunc().OStart();
  m_oend = winner->GetOutputFunc().OEnd();
  if (winner == m_winner) {
    ApproveChild(*winner);
  } else {
    if (m_winner) {
      m_log.debug("OutputFunc_Winner: un-winning (deleting) old winner " + string(*m_winner));
      DeleteChild(*m_winner);
    }
    InsertChild(*winner);
    m_winner = winner;
  }

  m_log.debug("OutputFunc_Winner " + string(*m_node) + " done update; hotlist has size " + boost::lexical_cast<string>(m_hotlist.Size()));
  m_log.debug(m_hotlist.Print());
}

/* OutputFunc_Sequence */

void OutputFunc_Sequence::operator() () {
  m_wasEmitting = m_emitting;
  m_emitting.clear();
  m_hotlist.Clear();
  m_ostart = NULL;
  m_oend = NULL;

  m_log.debug("OutputFunc_Sequence() " + string(*m_node) + ".   Has hotlist size: " + boost::lexical_cast<string>(m_hotlist.Size()));
  const State& state = m_node->GetState();
  if (!state.IsEmitting()) {
    return;
  }
  emitchildren_set wereEmit = m_emitChildren;
  emitchildren_set nowEmit;
  for (FWTree::child_iter i = m_node->children.begin(); i != m_node->children.end(); ++i) {
    m_log.debug("Considering child " + string(*i));
    const State& istate = i->GetState();
    if (!istate.IsEmitting()) {
      // Delete children that are no longer being emit
      for (emitchildren_iter we = wereEmit.begin(); we != wereEmit.end(); ++i) {
        m_log.debug("Deleting previously-emit child " + string(**we) + " from " + string(*i));
        DeleteChild(**we);
      }
      break;
    }
    OutputFunc& iof = i->GetOutputFunc();
    if ((iof.OStart() && !iof.OEnd()) || (!iof.OStart() && iof.OEnd())) {
      throw FWError("OutputFunc_Sequence() found " + string(*i) + " with only an ostart or oend, but not both");
    }
    nowEmit.insert(&*i);
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
    if (wereEmit.end() == wereEmit.find(&*i)) {
      InsertChild(*i);
    } else {
      ApproveChild(*i);
      wereEmit.erase(&*i);
    }
  }
  m_emitChildren = nowEmit;

  m_log.debug("OutputFunc_Sequence " + string(*m_node) + " done update; hotlist has size " + boost::lexical_cast<string>(m_hotlist.Size()));
  m_log.debug(m_hotlist.Print());
}

/* OutputFunc_Cap */

OutputFunc_Cap::OutputFunc_Cap(Log& log, auto_ptr<OutputFunc> outputFunc, const string& cap)
  : OutputFunc(log),
    m_outputFunc(outputFunc),
    m_cap(cap),
    m_capStart(cap),
    m_capEnd("/" + cap),
    m_isFirstTime(true) {
}

void OutputFunc_Cap::Init(const FWTree& x) {
  m_node = &x;
  m_outputFunc->Init(x);
}

void OutputFunc_Cap::operator() () {
  m_wasEmitting = m_emitting;
  m_emitting.clear();
  m_emitting.insert(&m_capStart);
  m_emitting.insert(&m_capEnd);
  m_hotlist.Clear();
  if (m_isFirstTime) {
    m_ostart = &m_capStart;
    m_oend = &m_capEnd;
    m_capStart.right = &m_capEnd;
    m_capEnd.left = &m_capStart;
    m_hotlist.Insert(m_capStart);
    m_hotlist.Insert(m_capEnd);
    m_isFirstTime = false;
  }

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

  m_log.debug("OutputFunc_Cap " + string(*m_node) + " done update; hotlist has size " + boost::lexical_cast<string>(m_hotlist.Size()));
  m_log.debug(m_hotlist.Print());
}
