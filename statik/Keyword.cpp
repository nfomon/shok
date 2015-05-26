// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Keyword.h"

#include "IncParser.h"
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
      MakeParseFunc_Keyword(str),
      MakeOutputFunc_Basic(name)));
}

auto_ptr<ParseFunc> statik::MakeParseFunc_Keyword(const string& str) {
  return auto_ptr<ParseFunc>(new ParseFunc_Keyword(str));
}

ParseFunc_Keyword::ParseFunc_Keyword(const string& str)
  : m_str(str) {
  if (m_str.empty()) {
    throw SError("Cannot create empty Keyword");
  }
}

void ParseFunc_Keyword::operator() (ParseAction::Action action, const List& inode, const STree* initiator) {
  g_log.info() << "Computing Keyword at " << *m_node << " with inode " << inode;
  State& state = m_node->GetState();
  state.Clear();
  string matched;
  bool done = false;
  const List* i = &m_node->IStart();
  for (; i != NULL; i = i->right) {
    m_node->GetIncParser().Listen(*m_node, *i);
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
