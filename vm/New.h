// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _New_h_
#define _New_h_

/* A New instruction: defines a symbol (name, type, value) */

#include "Expression.h"

#include "util/Log.h"

#include <boost/spirit/include/qi.hpp>
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;

#include <string>
#include <vector>

namespace vm {

struct New {
  std::string name;
  Expression exp;
};

template <typename Iterator>
struct NewParser : qi::grammar<Iterator, New(), ascii::space_type> {
public:
  NewParser() : NewParser::base_type(new_, "new parser") {
    using qi::int_;
    using qi::lit;
    using qi::graph;

    identifier_ %= qi::lexeme[ (qi::alpha | '_') >> *(qi::alnum | '_' | ':') ];
    new_ %=
      lit("(new")
      > identifier_
      > exp_
      > lit(")")
    ;
  }

private:
  ExpParser<Iterator> exp_;
  qi::rule<Iterator, New(), ascii::space_type> new_;
  qi::rule<Iterator, std::string(), ascii::space_type> identifier_;
};

}

#endif // _New_h_
