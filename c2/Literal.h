// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Literal_h_
#define _Literal_h_

/* Atomic literals */

#include "IntLiteral.h"
#include "StringLiteral.h"
#include "Scope.h"
#include "Type.h"

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>
namespace phoenix = boost::phoenix;
namespace spirit = boost::spirit;
namespace ascii = spirit::ascii;
namespace qi = spirit::qi;

#include <memory>
#include <string>
#include <vector>

namespace compiler {

typedef boost::variant<
  IntLiteral,
  StringLiteral
> Literal;

class Literal_bytecode : public boost::static_visitor<std::string> {
public:
  std::string operator() (const IntLiteral& lit) const { return lit.bytecode(); }
  std::string operator() (const StringLiteral& lit) const { return lit.bytecode(); }
};

class Literal_type : public boost::static_visitor<const Type&> {
public:
  const Type& operator() (const IntLiteral& lit) const { return lit.type(); }
  const Type& operator() (const StringLiteral& lit) const { return lit.type(); }
};

template <typename Iterator>
struct LiteralParser
  : qi::grammar<Iterator, Literal(const Scope&), ascii::space_type> {
public:
  LiteralParser()
    : LiteralParser::base_type(literal_, "Literal") {
    using phoenix::ref;
    using qi::_1;
    using qi::_r1;
    using qi::_val;
    using qi::char_;
    using qi::lit;

    literal_ %= (
      int_(_r1)
      | str_(_r1)
    );

    //BOOST_SPIRIT_DEBUG_NODE(literal_);
  }

private:
  IntLiteralParser<Iterator> int_;
  StringLiteralParser<Iterator> str_;
  qi::rule<Iterator, Literal(const Scope&), ascii::space_type> literal_;
};

}

#endif // _Literal_h_
