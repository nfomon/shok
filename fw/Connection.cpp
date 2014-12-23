// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Connection.h"

#include "FWTree.h"
#include "IList.h"

#include "util/Graphviz.h"
using Util::dotVar;

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace fw;

OConnection::OConnection(Log& log, const FWTree& x)
  : m_log(log),
    m_node(x),
    m_ostart(NULL),
    m_oend(NULL) {
}

void OConnection::ApproveChild(const FWTree& child) {
  m_log.debug("**** OConnection: " + string(m_node) + " Approving child " + string(child));
  const Hotlist::hotlist_vec& h = child.GetOConnection().GetHotlist();
  m_hotlist.Accept(h);
  const emitting_vec& e = child.GetOConnection().Emitting();
  m_emitting.insert(e.begin(), e.end());
}

void OConnection::InsertChild(const FWTree& child) {
  m_log.debug("**** OConnection: " + string(m_node) + " Inserting child " + string(child));
  const emitting_vec& e = child.GetOConnection().Emitting();
  for (emitting_iter i = e.begin(); i != e.end(); ++i) {
    m_hotlist.Insert(**i);
    m_emitting.insert(*i);
  }
}

void OConnection::DeleteChild(const FWTree& child) {
  m_log.debug("**** OConnection: " + string(m_node) + " Deleting child " + string(child));
  const emitting_vec& we = child.GetOConnection().WasEmitting();
  for (emitting_iter i = we.begin(); i != we.end(); ++i) {
    m_hotlist.Delete(**i);
    m_emitting.erase(*i);
  }
}

string OConnection::DrawOConnection(const string& context) const {
  string s;
/*
  if (m_ostart) {
    s += dotVar(m_ostart->owner, context) + " -> " + dotVar(&m_node, context) + " [constraint=false, weight=1, arrowsize=1.2, color=\"#22ee22\"];\n";
  }
*/
  for (emitting_iter i = m_emitting.begin(); i != m_emitting.end(); ++i) {
    s += dotVar((*i)->owner, context) + " -> " + dotVar(&m_node, context) + " [constraint=false, weight=1, arrowsize=1.2, color=\"#cc0044\"];\n";
  }
/*
  if (m_oend) {
    s += dotVar(m_ostart->owner, context) + " -> " + dotVar(&m_node, context) + " [constraint=false, weight=1, arrowsize=1.2, color=\"#ee2222\"];\n";
  }
*/
/*
  // Also draw the previous connections!
  for (emitting_iter i = m_wasEmitting.begin(); i != m_wasEmitting.end(); ++i) {
    s += dotVar((*i)->owner, context) + " -> " + dotVar(&m_node, context) + " [constraint=false, weight=1, arrowsize=1.2, color=\"#6600ee\"];\n";
  }
*/
  return s;
}

/* OConnectionSingle */

OConnectionSingle::OConnectionSingle(Log& log, const FWTree& x)
  : OConnection(log, x) {
    m_onode.reset(new IList(&x));
    m_ostart = m_onode.get();
    m_oend = m_onode.get();
    m_emitting.insert(m_onode.get());
    m_hotlist.Insert(*m_onode.get());
}

IList& OConnectionSingle::GetONode() const {
  if (!m_onode.get()) {
    throw FWError("Cannot get ONode for update; no ONode found");
  }
  return *m_onode.get();
}

void OConnectionSingle::ONodeUpdated() {
  if (!m_onode.get()) {
    throw FWError("ONodeUpdated() called but there is no ONode assigned");
  }
  m_hotlist.Update(*m_onode.get());
}

/* OConnectionWinner */

void OConnectionWinner::DeclareWinner(const FWTree& winner) {
  m_log.debug("**** OConnection: " + string(m_node) + " Declaring winner " + string(winner));
  m_ostart = winner.GetOConnection().OStart();
  m_oend = winner.GetOConnection().OEnd();
  if (&winner == m_winner) {
    ApproveChild(winner);
  } else {
    if (m_winner) {
      m_log.debug("OConnection: un-winning (deleting) old winner " + string(*m_winner));
      DeleteChild(*m_winner);
    }
    InsertChild(winner);
    m_winner = &winner;
  }
}

/* OConnectionSequence */

void OConnectionSequence::AddNextChild(const FWTree& child, bool isNew) {
  m_log.debug("**** OConnection: " + string(m_node) + " Adding next child " + string(child) + (isNew ? ", new" : ""));
  if (!!child.GetOConnection().OStart() ^ !!child.GetOConnection().OEnd()) {
    throw FWError("OConnectionSequence::AddNextChild found " + string(child) + " with only an ostart or oend, but not both");
  }
  if (!child.GetOConnection().OStart()) {
    return;
  }
  if (!m_ostart) {
    m_ostart = child.GetOConnection().OStart();
    if (m_wasEmitting.empty()) {
      isNew = true;   // bit of a hack around the Reposition() case
    }
  } else {
    child.GetOConnection().OStart()->left = m_oend;
    m_oend->right = child.GetOConnection().OStart();
  }
  m_oend = child.GetOConnection().OEnd();
  if (isNew) {
    InsertChild(child);
  } else {
    ApproveChild(child);
  }
}
