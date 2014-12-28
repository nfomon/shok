// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "OutputStrategy.h"

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

OutputStrategy::OutputStrategy(Log& log, const FWTree& x)
  : m_log(log),
    m_node(x),
    m_ostart(NULL),
    m_oend(NULL) {
}

void OutputStrategy::ApproveChild(const FWTree& child) {
  m_log.debug("**** OutputStrategy: " + string(m_node) + " Approving child " + string(child));
  const Hotlist::hotlist_vec& h = child.GetOutputStrategy().GetHotlist();
  m_hotlist.Accept(h);
  const emitting_set& e = child.GetOutputStrategy().Emitting();
  m_emitting.insert(e.begin(), e.end());
}

void OutputStrategy::InsertChild(const FWTree& child) {
  m_log.debug("**** OutputStrategy: " + string(m_node) + " Inserting child " + string(child));
  const emitting_set& e = child.GetOutputStrategy().Emitting();
  for (emitting_iter i = e.begin(); i != e.end(); ++i) {
    m_hotlist.Insert(**i);
    m_emitting.insert(*i);
  }
}

void OutputStrategy::DeleteChild(const FWTree& child) {
  m_log.debug("**** OutputStrategy: " + string(m_node) + " Deleting child " + string(child));
  const emitting_set& we = child.GetOutputStrategy().WasEmitting();
  for (emitting_iter i = we.begin(); i != we.end(); ++i) {
    m_hotlist.Delete(**i);
    m_emitting.erase(*i);
  }
}

/*
string OutputStrategy::DrawEmitting(const string& context) const {
  string s;
  if (m_ostart) {
    s += dotVar(m_ostart->owner, context) + " -> " + dotVar(&m_node, context) + " [constraint=false, weight=1, arrowsize=1.2, color=\"#22ee22\"];\n";
  }
  for (emitting_iter i = m_emitting.begin(); i != m_emitting.end(); ++i) {
    s += dotVar((*i)->owner, context) + " -> " + dotVar(&m_node, context) + " [constraint=false, weight=1, arrowsize=1.2, color=\"#cc0044\"];\n";
  }
  if (m_oend) {
    s += dotVar(m_ostart->owner, context) + " -> " + dotVar(&m_node, context) + " [constraint=false, weight=1, arrowsize=1.2, color=\"#ee2222\"];\n";
  }
  // Also draw the previous connections!
  for (emitting_iter i = m_wasEmitting.begin(); i != m_wasEmitting.end(); ++i) {
    s += dotVar((*i)->owner, context) + " -> " + dotVar(&m_node, context) + " [constraint=false, weight=1, arrowsize=1.2, color=\"#6600ee\"];\n";
  }
  return s;
}
*/

/* OutputStrategySingle */

OutputStrategySingle::OutputStrategySingle(Log& log, const FWTree& x)
  : OutputStrategy(log, x),
    m_onode(x.GetRule().Name()) {
    m_ostart = &m_onode;
    m_oend = &m_onode;
    m_emitting.insert(&m_onode);
    m_hotlist.Insert(m_onode);
}

void OutputStrategySingle::Update() {
/*
  const State& state = m_node.GetState();
  if (!state.IsEmitting()) {
    m_wasEmitting = false;
    return;
  }
  if (!m_wasEmitting) {
    m_hotlist.Insert(m_onode);
  }
  m_wasEmitting = true;
*/
}

/* OutputStrategyValue */

void OutputStrategyValue::Update() {
/*
  const State& state = m_node.GetState();
  if (!state.IsEmitting()) {
    m_wasEmitting = false;
    return;
  }
*/
  string value;
  for (const IList* i = &m_node.IStart(); i != NULL; i = i->right) {
    value += i->value;
  }
//  if (m_wasEmitting && m_onode.value != value) {
    m_onode.value = value;
    m_hotlist.Update(m_onode);
/*
  } else if (!m_wasEmitting) {
    m_hotlist.Insert(m_onode);
  }
  m_wasEmitting = true;
*/
}

/* OutputStrategyWinner */

void OutputStrategyWinner::Clear() {
  m_winner = NULL;
  m_ostart = NULL;
  m_oend = NULL;
}

void OutputStrategyWinner::Reset() {
  m_wasEmitting = m_emitting;
  m_emitting.clear();
  m_hotlist.Clear();
}

void OutputStrategyWinner::Update() {
  const State& state = m_node.GetState();
  if (!state.IsEmitting()) {
    m_log.debug("OutputStrategyWinner " + string(m_node) + " is not emitting; skipping update");
    return;
  }
  bool isComplete = state.IsComplete();
  const FWTree* winner = NULL;
  for (FWTree::child_iter i = m_node.children.begin(); i != m_node.children.end(); ++i) {
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
    throw FWError("OutputStrategyWinner for " + string(m_node) + " failed to find winner");
  }
  m_log.debug("**** OutputStrategyWinner: " + string(m_node) + " Declaring winner " + string(*winner));
  m_ostart = winner->GetOutputStrategy().OStart();
  m_oend = winner->GetOutputStrategy().OEnd();
  if (winner == m_winner) {
    ApproveChild(*winner);
  } else {
    if (m_winner) {
      m_log.debug("OutputStrategyWinner: un-winning (deleting) old winner " + string(*m_winner));
      DeleteChild(*m_winner);
    }
    InsertChild(*winner);
    m_winner = winner;
  }

  m_log.debug("OutputStrategyWinner " + string(m_node) + " done update; hotlist has size " + boost::lexical_cast<string>(m_hotlist.GetHotlist().size()));
  m_log.debug(m_hotlist.Print());
}

/* OutputStrategySequence */

void OutputStrategySequence::Clear() {
  m_emitting.clear();
  m_ostart = NULL;
  m_oend = NULL;
}

void OutputStrategySequence::Reset() {
  m_wasEmitting = m_emitting;
  m_emitting.clear();
  m_hotlist.Clear();
  m_ostart = NULL;
  m_oend = NULL;
}

void OutputStrategySequence::Update() {
  m_log.debug("OutputStrategySequence::Update " + string(m_node));
  const State& state = m_node.GetState();
  if (!state.IsEmitting()) {
    return;
  }
  emitchildren_set wereEmit = m_emitChildren;
  emitchildren_set nowEmit;
  for (FWTree::child_iter i = m_node.children.begin(); i != m_node.children.end(); ++i) {
    const State& istate = i->GetState();
    if (!istate.IsEmitting()) {
      // Delete children that are no longer being emit
      for (emitchildren_iter we = wereEmit.begin(); we != wereEmit.end(); ++i) {
        DeleteChild(**we);
      }
      break;
    }
    OutputStrategy& iostrat = i->GetOutputStrategy();
    if ((iostrat.OStart() && !iostrat.OEnd()) || (!iostrat.OStart() && iostrat.OEnd())) {
      throw FWError("OutputStrategySequence::Update found " + string(*i) + " with only an ostart or oend, but not both");
    }
    nowEmit.insert(&*i);
    if (!iostrat.OStart()) {
      continue;
    }
    if (!m_ostart) {
      m_ostart = iostrat.OStart();
    } else {
      iostrat.OStart()->left = m_oend;
      m_oend->right = iostrat.OStart();
    }
    m_oend = iostrat.OEnd();
    if (wereEmit.end() == wereEmit.find(&*i)) {
      InsertChild(*i);
    } else {
      ApproveChild(*i);
      wereEmit.erase(&*i);
    }
  }
  m_emitChildren = nowEmit;

  m_log.debug("OutputStrategySequence " + string(m_node) + " done update; hotlist has size " + boost::lexical_cast<string>(m_hotlist.GetHotlist().size()));
  m_log.debug(m_hotlist.Print());
}
