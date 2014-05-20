// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Identifier_h_
#define _Identifier_h_

/* Identifier */

#include <boost/spirit/include/qi.hpp>
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;

#include <string>

namespace vm {

template <typename Iterator>
struct IdentifierParser : qi::grammar<Iterator, std::string()> {
public:
  IdentifierParser() : IdentifierParser::base_type(id_, "Identifier") {
    using qi::char_;

    id_ %= char_("A-Za-z_:@") > *char_("0-9A-Za-z_:@");

    //BOOST_SPIRIT_DEBUG_NODE(id_);
  }

private:
  qi::rule<Iterator, std::string()> id_;
};

}

#endif // _Identifier_h_
