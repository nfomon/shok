// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _New_h_
#define _New_h_

/* A New instruction: defines a symbol (name, type, value) */

#include "Member.h"

#include "util/Log.h"

#include <boost/spirit/include/qi.hpp>
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;

#include <string>
#include <vector>

namespace vm {

struct New {
  typedef std::vector<Member> member_vec;
  typedef member_vec::const_iterator member_iter;
  static const std::string NO_SOURCE;

  std::string name;
  std::string source;
  member_vec members;
};

template <typename Iterator>
struct NewParser : qi::grammar<Iterator, New(), ascii::space_type> {
  NewParser() : NewParser::base_type(start) {
    using qi::int_;
    using qi::lit;
    using qi::graph;

    identifier %= qi::lexeme[ (qi::alpha | '_') >> *(qi::alnum | '_' | ':') ];
    blank %= qi::lexeme[New::NO_SOURCE];
    start %=
      lit("new")
      >> identifier
      >> (identifier | blank)
      >> *member_
      >> lit("endnew")
    ;
  }

  qi::rule<Iterator, New(), ascii::space_type> start;
  qi::rule<Iterator, std::string(), ascii::space_type> identifier;
  qi::rule<Iterator, std::string(), ascii::space_type> blank;
  MemberParser<Iterator> member_;
};

}

#endif // _New_h_
