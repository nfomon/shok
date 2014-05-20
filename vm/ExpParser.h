// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _ExpParser_h_
#define _ExpParser_h_

/* Expression parser */

#include "Expression.h"
#include "Identifier.h"
#include "InstructionParser.h"
#include "VMError.h"

#include "util/Log.h"

#include <boost/fusion/include/sequence.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include <boost/variant.hpp>
namespace fusion = boost::fusion;
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;

#include <string>
#include <vector>

namespace vm {

template <typename Iterator>
struct ExpParser : qi::grammar<Iterator, Expression(), ascii::space_type> {
public:
  ExpParser()
    : ExpParser::base_type(exp_, "Expression"),
      instruction_(*this) {
    using qi::char_;
    using qi::lexeme;
    using qi::lit;
    using qi::graph;

    methodcall_ = lit("(call") > exp_ > identifier_ > *exp_ > lit(')');
    member_ = lit("(member") > identifier_ > identifier_ > lit(')');
    object_ = lit("(object") > *member_ > lit(')');
    function_ = lit("(function") > *instruction_ > lit(')');
    exp_ %= (
      identifier_
      | methodcall_
      | object_
      | function_
    );

    //BOOST_SPIRIT_DEBUG_NODE(methodcall_);
    //BOOST_SPIRIT_DEBUG_NODE(member_);
    //BOOST_SPIRIT_DEBUG_NODE(object_);
    //BOOST_SPIRIT_DEBUG_NODE(exp_);
  }

private:
  IdentifierParser<Iterator> identifier_;
  InstructionParser<Iterator> instruction_;
  qi::rule<Iterator, MethodCall(), ascii::space_type> methodcall_;
  qi::rule<Iterator, ObjectLiteral::member_pair(), ascii::space_type> member_;
  qi::rule<Iterator, ObjectLiteral(), ascii::space_type> object_;
  qi::rule<Iterator, FunctionLiteral(), ascii::space_type> function_;
  qi::rule<Iterator, Expression(), ascii::space_type> exp_;
};

}

#endif // _ExpParser_h_
