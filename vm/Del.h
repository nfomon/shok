// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Del_h_
#define _Del_h_

/* Del instruction: delete an object */

#include "Expression.h"
#include "Object.h"

#include "util/Log.h"

#include <boost/utility.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;

#include <string>
#include <vector>

// debug
#include <iostream>
using std::cout;
using std::endl;

namespace vm {

struct Exec_Del {
public:
  Exec_Del(symbol_map& symbols)
    : m_symbols(symbols) {}

  void operator() (const std::string& name, qi::unused_type, qi::unused_type) const;

private:
  symbol_map& m_symbols;
};

template <typename Iterator>
struct DelParser : qi::grammar<Iterator, std::string(), ascii::space_type> {
public:
  DelParser() : DelParser::base_type(del_, "del parser") {
    using qi::char_;
    using qi::lexeme;
    using qi::lit;

    identifier_ %= lexeme[ char_("A-Za-z_@") > *char_("0-9A-Za-z_:") ];
    del_ %=
      lit("(del")
      > identifier_
      > lit(')')
    ;
  }

private:
  qi::rule<Iterator, std::string(), ascii::space_type> del_;
  qi::rule<Iterator, std::string(), ascii::space_type> identifier_;
};

}

#endif // _Del_h_
