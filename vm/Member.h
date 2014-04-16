// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Member_h_
#define _Member_h_

/* An object member */

#include "util/Log.h"

#include <boost/variant/recursive_wrapper.hpp>
#include <boost/spirit/include/qi.hpp>
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;

#include <string>
#include <vector>

namespace vm {

struct Member {
  typedef std::vector<boost::recursive_wrapper<Member> > member_vec;
  typedef member_vec::const_iterator member_iter;
  static const std::string NO_SOURCE;

  std::string name;
  std::string source;
  std::vector<boost::recursive_wrapper<Member> > members;
};

template <typename Iterator>
struct MemberParser : qi::grammar<Iterator, Member(), ascii::space_type> {
  MemberParser() : MemberParser::base_type(start) {
    using qi::int_;
    using qi::lit;
    using qi::graph;

    identifier %= qi::lexeme[ (qi::alpha | '_') >> *(qi::alnum | '_' | ':') ];
    blank %= qi::lexeme[Member::NO_SOURCE];
    start %=
      lit("member")
      >> identifier
      >> (identifier | blank)
      >> *start
      >> lit("endmember")
    ;
  }

  qi::rule<Iterator, Member(), ascii::space_type> start;
  qi::rule<Iterator, std::string(), ascii::space_type> identifier;
  qi::rule<Iterator, std::string(), ascii::space_type> blank;
};

}

#endif // _Member_h_
