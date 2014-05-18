// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _New_h_
#define _New_h_

/* A New instruction: defines a symbol (name, type, value) */

#include "Context.h"
#include "Expression.h"

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

template <typename Iterator>
struct NewParser : qi::grammar<Iterator, New(), ascii::space_type> {
public:
  NewParser() : NewParser::base_type(new_, "New") {
    using qi::char_;
    using qi::lexeme;
    using qi::lit;
    using qi::graph;

    identifier_ %= lexeme[ char_("A-Za-z_") > *char_("0-9A-Za-z_:") ];
    new_ %=
      lit("(new")
      > identifier_
      > exp_
      > lit(')')
    ;

    //BOOST_SPIRIT_DEBUG_NODE(new_);
  }

private:
  ExpParser<Iterator> exp_;
  qi::rule<Iterator, New(), ascii::space_type> new_;
  qi::rule<Iterator, std::string(), ascii::space_type> identifier_;
};

}

#endif // _New_h_
