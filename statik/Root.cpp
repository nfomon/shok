// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Root.h"

#include "Connector.h"
#include "OutputFunc.h"
#include "SError.h"
#include "SLog.h"
#include "STree.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace statik;

auto_ptr<ComputeFunc> statik::MakeComputeFunc_Root(const string& name) {
  return auto_ptr<ComputeFunc>(new ComputeFunc_Root(name));
}

ComputeFunc_Root::ComputeFunc_Root(const string& name)
  : m_name(name) {
}

void ComputeFunc_Root::operator() (ParseAction::Action action, const IList& inode, const STree* initiator) {
  g_log.info() << "Computing Root at " << *m_node << " with inode " << inode << " and action " << ParseAction::UnMapAction(action);
  State& state = m_node->GetState();
  if (m_node->children.empty()) {
    if (inode.right && !inode.right->left) {
      m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, *inode.right);
      state.GoPending();
      return;
    } else {
      m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, inode);
      state.GoPending();
      return;
    }
  }
  if (ParseAction::Restart == action) {
    m_node->GetConnector().Enqueue(ParseAction(ParseAction::Restart, **m_node->children.begin(), inode, m_node));
    state.GoPending();
    return;
  }
  const STree& child = **m_node->children.begin();
  const State& childState = child.GetState();
  if (child.IsClear()) {
    // This is wrong.  Supposed to set to m_firstINode but we removed that because tricky to maintain lol...
    m_node->children.erase(m_node->children.begin());
    if (inode.right && inode.right->left != &inode) {
      m_node->GetConnector().Enqueue(ParseAction(ParseAction::Restart, *m_node, *inode.right, m_node));
      state.GoPending();
      return;
    } else {
      state.GoBad();
      m_node->GetIConnection().SetEnd(m_node->IStart());
    }
  } else if (childState.IsPending()) {
    throw SError("Root node's child " + string(child) + " is pending");
  } else if (childState.IsBad()) {
    state.GoBad();
    m_node->GetIConnection().SetEnd(child.IEnd());
  } else if (childState.IsOK()) {
    state.GoOK();
    m_node->GetIConnection().SetEnd(child.IEnd());
  } else if (childState.IsDone()) {
    state.GoDone();
    m_node->GetIConnection().SetEnd(child.IEnd());
  } else if (childState.IsComplete()) {
    state.GoComplete();
    m_node->GetIConnection().SetEnd(child.IEnd());
  } else {
    throw SError("Cannot determine state of Root node when Child's state is unknown.  Child: " + string(child));
  }

  g_log.debug() << "Root now at: " << *m_node << " with IStart: " << m_node->IStart() << " and IEnd: " << m_node->IEnd();
}
