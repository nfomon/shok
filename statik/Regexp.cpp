// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Regexp.h"

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

auto_ptr<Rule> statik::REGEXP(const string& name, const boost::regex& regex) {
  return auto_ptr<Rule>(new Rule(name,
      MakeParseFunc_Regexp(regex),
      MakeOutputFunc_IValues(name)));
}

auto_ptr<ParseFunc> statik::MakeParseFunc_Regexp(const boost::regex& regex) {
  return auto_ptr<ParseFunc>(new ParseFunc_Regexp(regex));
}

ParseFunc_Regexp::ParseFunc_Regexp(const boost::regex& regex)
  : m_regex(regex) {
  if (m_regex.empty()) {
    throw SError("Cannot create Regexp with empty regex");
  }
}

void ParseFunc_Regexp::operator() (ParseAction::Action action, const List& inode, const STree* initiator) {
  g_log.info() << "Computing Regexp at " << *m_node << " with inode " << inode;
  State& state = m_node->GetState();
  state.Clear();
  string str;
  const List* i = &m_node->IStart();
  state.GoOK();
  for (; i != NULL; i = i->right) {
    m_node->GetIncParser().Listen(*m_node, *i);
    m_node->GetIConnection().SetEnd(*i);
    str += i->value;
    if (boost::regex_match(str, m_regex)) {
      state.GoDone();
      // keep going if possible, in case we can get complete
    } else {
      if (str.size() > 1) {
        if (boost::regex_match(str.begin(), str.end()-1, m_regex)) {
          state.GoComplete();
          break;
        }
      }
      state.GoBad();
      break;
    }
  }
  if (state.IsPending()) {
    throw SError("Regexp failed to determine state");
  }
  g_log.debug() << "Regexp now at: " << *m_node << " with IStart: " << m_node->IStart() << " and IEnd: " << m_node->IEnd();
}
