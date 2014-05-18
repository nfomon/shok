// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Call_h_
#define _Call_h_

/* A "procedure" call, i.e. a function call that isn't a method call. */

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

namespace vm {

struct Call {
  Expression function;
  std::vector<Expression> args;
};

struct Exec_Call {
public:
  Exec_Call(symbol_map& symbols)
    : m_symbols(symbols) {}

  void operator() (const Call& n, qi::unused_type, qi::unused_type) const;

private:
  symbol_map& m_symbols;
};

template <typename Iterator>
struct CallParser : qi::grammar<Iterator, Call(), ascii::space_type> {
public:
  CallParser() : CallParser::base_type(call_, "Call") {
    using qi::char_;
    using qi::lexeme;
    using qi::lit;
    using qi::graph;

    call_ %=
      lit("(call")
      > exp_
      > *exp_
      > lit(')')
    ;

    //BOOST_SPIRIT_DEBUG_NODE(call_);
  }

private:
  ExpParser<Iterator> exp_;
  qi::rule<Iterator, Call(), ascii::space_type> call_;
};

}

#endif // _Call_h_
