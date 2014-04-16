// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Executor.h"

#include "Member.h"
#include "New.h"
#include "Object.h"
#include "VMError.h"

#include "util/Util.h"

#include <boost/bind.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;

#include <istream>
#include <string>
#include <utility>
#include <vector>
using std::istream;
using std::string;
using std::vector;

// debug
#include <iostream>
using std::cout;
using std::endl;

using namespace vm;

Executor::Executor(Log& log, istream& input)
  : m_log(log),
    m_input(input),
    on_new(boost::bind(&Executor::exec_new, this, _1)) {
}

void Executor::exec_new(const New& n) {
  cout << "New: name=" << n.name << " source=" << n.source << endl;
  for (New::member_iter i = n.members.begin(); i != n.members.end(); ++i) {
    cout << " - member " << i->name << ":" << i->source << endl;
  }
  if (m_symbolTable.find(n.name) != m_symbolTable.end()) {
    throw VMError("Cannot insert symbol " + n.name + "; already exists");
  }
  if (New::NO_SOURCE == n.source) {
    m_symbolTable.insert(std::make_pair(n.name, new Object(NULL)));
  } else {
    symbol_iter s = m_symbolTable.find(n.source);
    if (m_symbolTable.end() == s) {
      throw VMError("Source " + n.source + " not found for symbol " + n.name);
    }
    m_symbolTable.insert(std::make_pair(n.name, new Object(s->second)));
  }
}

BOOST_FUSION_ADAPT_STRUCT(
  Member,
  (std::string, name)
  (std::string, source)
  (Member::member_vec, members)
)

BOOST_FUSION_ADAPT_STRUCT(
  New,
  (std::string, name)
  (std::string, source)
  (New::member_vec, members)
)

bool Executor::execute() {
  // iterate over stream input
  typedef std::istreambuf_iterator<char> base_iterator_type;
  base_iterator_type in_begin(m_input);

  // convert input iterator to forward iterator, usable by spirit parser
  typedef spirit::multi_pass<base_iterator_type> forward_iterator_type;
  forward_iterator_type fwd_begin = spirit::make_default_multi_pass(in_begin);
  forward_iterator_type fwd_end;

  NewParser<forward_iterator_type> new_;
  typedef qi::rule<forward_iterator_type, ascii::space_type> Rule;

  Rule Statement_ = +(new_[on_new]);
  Rule Bytecode_ = +Statement_;

  bool r = qi::phrase_parse(
    fwd_begin, fwd_end,
    Bytecode_,
    ascii::space
  );

  if (!r || fwd_begin != fwd_end) {
    return false;
  }
  return r;
}
