// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Executor.h"

#include "Expression.h"
#include "New.h"
#include "Object.h"
#include "VMError.h"

#include "util/Util.h"

#include <boost/bind.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

namespace phoenix = boost::phoenix;
namespace spirit = boost::spirit;
namespace ascii = spirit::ascii;
namespace qi = spirit::qi;

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
    m_input(input) {
}

void Executor::exec_new(const New& n) {
  cout << "New: name=" << n.name << endl;
  if (m_symbolTable.find(n.name) != m_symbolTable.end()) {
    throw VMError("Cannot insert symbol " + n.name + "; already exists");
  }
/*
  if (New::NO_SOURCE == n.source) {
    m_symbolTable.insert(std::make_pair(n.name, new Object(NULL)));
  } else {
    symbol_iter s = m_symbolTable.find(n.source);
    if (m_symbolTable.end() == s) {
      throw VMError("Source " + n.source + " not found for symbol " + n.name);
    }
    m_symbolTable.insert(std::make_pair(n.name, new Object(s->second)));
  }
*/
}

BOOST_FUSION_ADAPT_STRUCT(
  New,
  (std::string, name)
  (Expression, exp)
)

// We need to tell fusion about our mini_xml struct
// to make it a first-class fusion citizen
BOOST_FUSION_ADAPT_STRUCT(
  MethodCall,
  (Expression, source)
  (std::string, method)
  (std::vector<Expression>, args)
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

  Rule Statement_ = new_[phoenix::bind(&Executor::exec_new, this, qi::_1)];
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
