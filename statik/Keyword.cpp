// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Keyword.h"

#include "OutputFunc.h"
#include "SError.h"
#include "SLog.h"
#include "STree.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace statik;

auto_ptr<Rule> statik::KEYWORD(const string& str) {
  return statik::KEYWORD(str, str);
}

auto_ptr<Rule> statik::KEYWORD(const string& name, const string& str) {
  return auto_ptr<Rule>(new Rule(name,
      MakeComputeFunc_Keyword(str),
      MakeOutputFunc_Basic(name)));
}

auto_ptr<ComputeFunc> statik::MakeComputeFunc_Keyword(const string& str) {
  return auto_ptr<ComputeFunc>(new ComputeFunc_Keyword(str));
}

ComputeFunc_Keyword::ComputeFunc_Keyword(const string& str)
  : m_str(str) {
  if (m_str.empty()) {
    throw SError("Cannot create empty Keyword");
  }
}

void ComputeFunc_Keyword::operator() (ConnectorAction::Action action, const IList& inode, const STree* initiator) {
  g_log.info() << "Computing Keyword at " << *m_node << " with inode " << inode;
  State& state = m_node->GetState();
  state.Clear();
  string matched;
  bool done = false;
  const IList* i = &m_node->IStart();
  for (; i != NULL; i = i->right) {
    m_node->GetConnector().Listen(*m_node, *i);
    m_node->GetIConnection().SetEnd(*i);
    if (done) {
      state.GoComplete();
      break;
    }
    matched += i->value;
    if (m_str == matched) {
      state.GoDone();
      done = true;
    } else if (m_str.substr(0, matched.size()) != matched) {
      state.GoBad();
      break;
    } else {
      // Just ok; keep going (if possible)
      state.GoOK();
    }
  }
  if (state.IsEmitting()) {
    state.Lock();
  } else {
    state.Unlock();
  }
  if (state.IsPending()) {
    throw SError("Keyword failed to determine state");
  }
  g_log.debug() << "Keyword now at: " << *m_node << " with IStart: " << m_node->IStart() << " and IEnd: " << m_node->IEnd();
}
