// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _New_h_
#define _New_h_

/* A New instruction: defines a symbol (name, type, value) */

#include "Context.h"
#include "Expression.h"
#include "Identifier.h"

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

namespace vm {

struct New {
  void exec(Context& context) const;
  std::string name;
  Expression exp;
};

template <typename Iterator> struct ExpParser;

template <typename Iterator>
struct NewParser : qi::grammar<Iterator, New(), ascii::space_type> {
public:
  NewParser(ExpParser<Iterator>& exp_)
    : NewParser::base_type(new_, "New"),
      exp_(exp_) {
    using qi::char_;
    using qi::lexeme;
    using qi::lit;
    using qi::graph;

    new_ %=
      lit("(new")
      > identifier_
      > exp_
      > lit(')')
    ;

    //BOOST_SPIRIT_DEBUG_NODE(new_);
  }

private:
  ExpParser<Iterator>& exp_;

  IdentifierParser<Iterator> identifier_;
  qi::rule<Iterator, New(), ascii::space_type> new_;
};

}

#endif // _New_h_
