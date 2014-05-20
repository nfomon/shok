// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Del_h_
#define _Del_h_

/* Del instruction: delete an object */

#include "Context.h"
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

struct Del {
  void exec(Context& context) const;
  std::string name;
};

template <typename Iterator>
struct DelParser : qi::grammar<Iterator, Del(), ascii::space_type> {
public:
  DelParser() : DelParser::base_type(del_, "Del") {
    using qi::char_;
    using qi::lexeme;
    using qi::lit;

    del_ %=
      lit("(del")
      > identifier_
      > lit(')')
    ;

    //BOOST_SPIRIT_DEBUG_NODE(del_);
  }

private:
  IdentifierParser<Iterator> identifier_;
  qi::rule<Iterator, Del(), ascii::space_type> del_;
};

}

#endif // _Del_h_
