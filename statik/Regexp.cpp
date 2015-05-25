// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Regexp.h"

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

auto_ptr<Rule> statik::REGEXP(const string& name, const boost::regex& regex) {
  return auto_ptr<Rule>(new Rule(name,
      MakeComputeFunc_Regexp(regex),
      MakeOutputFunc_IValues(name)));
}

auto_ptr<ComputeFunc> statik::MakeComputeFunc_Regexp(const boost::regex& regex) {
  return auto_ptr<ComputeFunc>(new ComputeFunc_Regexp(regex));
}

ComputeFunc_Regexp::ComputeFunc_Regexp(const boost::regex& regex)
  : m_regex(regex) {
  if (m_regex.empty()) {
    throw SError("Cannot create Regexp with empty regex");
  }
}

void ComputeFunc_Regexp::operator() (ConnectorAction::Action action, const IList& inode, const STree* initiator) {
  g_log.info() << "Computing Regexp at " << *m_node;
  State& state = m_node->GetState();
  state.Clear();
  string str;
  const IList* i = &m_node->IStart();
  for (; i != NULL; i = i->right) {
    m_node->GetConnector().Listen(*m_node, *i);
    m_node->GetIConnection().SetEnd(*i);
    str += i->value;
    if (!boost::regex_match(str, m_regex)) {
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
  if (NULL == i) {
    boost::match_results<string::const_iterator> match_result;
    if (boost::regex_match(str, match_result, m_regex, boost::match_default | boost::match_partial)
        && match_result[0].matched) {
      state.GoDone();
    }
  }
  g_log.debug() << "Regexp now at " << *m_node;
}
