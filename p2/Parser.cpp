// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Parser.h"

#include "Shok.h"

#include <boost/bind.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>
namespace phoenix = boost::phoenix;
namespace spirit = boost::spirit;
namespace ascii = spirit::ascii;
namespace qi = spirit::qi;

#include <istream>
#include <ostream>
#include <string>
using std::endl;
using std::istream;
using std::ostream;
using std::string;

using namespace parser;

Parser::Parser(istream& input, ostream& output)
  : m_input(input),
    m_output(output) {
}

bool Parser::parse() {
  // iterate over stream input
  typedef std::istreambuf_iterator<char> base_iterator_type;
  base_iterator_type in_begin(m_input);

  // convert input iterator to forward iterator, usable by spirit parser
  typedef spirit::multi_pass<base_iterator_type> forward_iterator_type;
  forward_iterator_type fwd_begin = spirit::make_default_multi_pass(in_begin);
  forward_iterator_type fwd_end;

  ShokParser<forward_iterator_type> shok_(m_output);

  //BOOST_SPIRIT_DEBUG_NODE(shok_);

  bool r = qi::phrase_parse(
    fwd_begin, fwd_end,
    shok_,
    ascii::space
  );

  if (!r || fwd_begin != fwd_end) {
    return false;
  }

  return r;
}
