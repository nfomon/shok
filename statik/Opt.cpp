// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Opt.h"

#include "IncParser.h"
#include "OutputFunc.h"
#include "SLog.h"
#include "STree.h"

#include <memory>
#include <string>
#include <vector>
using std::auto_ptr;
using std::string;
using std::vector;

using namespace statik;

auto_ptr<Rule> statik::OPT(const string& name) {
  return auto_ptr<Rule>(new Rule(name,
      MakeParseFunc_Opt(),
      MakeOutputFunc_Root()));  // FIXME hack?
}

auto_ptr<ParseFunc> statik::MakeParseFunc_Opt() {
  return auto_ptr<ParseFunc>(new ParseFunc_Opt());
}

void ParseFunc_Opt::operator() (ParseAction::Action action, const List& inode, const STree* initiator) {
  // Process
  g_log.debug() << "Parsing Opt at " << *m_node;

  if (ParseAction::Restart == action) {
    if (m_node->children.empty()) {
      if (m_node->GetRule().GetChildren().size() != 1) {
        throw SError("Opt rule must have exactly one child");
      }
      (void) m_node->GetRule().GetChildren().at(0)->MakeNode(*m_node, inode);
    } else {
      STree& child = *m_node->children.at(0);
      child.GetIncParser().Enqueue(ParseAction(ParseAction::Restart, child, inode));
    }
    return;
  }
  if (m_node->children.empty()) {
    throw SError("Cannot parse Opt at " + m_node->Print() + " that has no children");
  } else if (action != ParseAction::ChildUpdate) {
    throw SError("Failed to process non-ChildUpdate action in Opt: " + m_node->Print());
  }

  // Compute
  const STree& child = *m_node->children.at(0);
  State& state = m_node->GetState();
  const State& child_state = child.GetState();
  if (child.IsClear()) {
    g_log.info() << "Opt: " << *m_node << " child " << child << " is clear - clearing self";
    m_node->ClearNode(inode);
    return;
  }
  if (child_state.IsLocked()) {
    g_log.info() << "Opt: " << *m_node << " child " << child << " is locked - locking self";
    state.Lock();
  }
  if (child_state.IsPending()) {
    throw SError("Opt: " + m_node->Print() + " cannot compute with pending child");
  } else if (child_state.IsBad()) {
    g_log.info() << "Opt: " << *m_node << " going complete since child is bad";
    state.GoComplete();
    m_node->GetIConnection().SetEnd(m_node->IStart());
  } else {
    state.SetStation(child_state.GetStation());
    m_node->GetIConnection().SetEnd(child.IEnd());
  }
  g_log.debug() << "Opt " << *m_node << " now has child: " << child << " with IStart: " << m_node->IStart() << " and IEnd: " << m_node->IEnd();
}
