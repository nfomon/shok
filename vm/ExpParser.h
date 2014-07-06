// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _ExpParser_h_
#define _ExpParser_h_

/* Expression parser */

#include "Expression.h"
#include "Identifier.h"
#include "InstructionParser.h"
#include "VMError.h"

#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;

#include <string>
#include <vector>

namespace vm {

typedef boost::variant<
  IntLiteral,
  StringLiteral
> Literal;

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

    nums_ %= +char_("0-9");
    int_ %=
      lit("(int")
      > nums_
      > lit(')');

    sq_inner_ %= +(~char_("\\\'") | (char_('\\') > char_));
    dq_inner_ %= +(~char_("\\\"") | (char_('\\') > char_));
    sq_ %= (char_('\'') > sq_inner_ > char_('\''));
    dq_ %= (char_('\"') > dq_inner_ > char_('\"'));
    text_ %= sq_ | dq_;
    str_ %=
      lit("(str")
      > text_
      > lit(')');

    methodcall_ = lit("(call") > exp_ > identifier_ > *exp_ > lit(')');
    member_ = lit("(member") > identifier_ > identifier_ > lit(')');
    object_ = lit("(object") > *member_ > lit(')');
    function_ = lit("(function") > *instruction_ > lit(')');
    exp_ %= (
      identifier_
      | int_
      | str_
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
  qi::rule<Iterator, std::string()> nums_;
  qi::rule<Iterator, IntLiteral(), ascii::space_type> int_;
  qi::rule<Iterator, std::string()> sq_inner_;
  qi::rule<Iterator, std::string()> dq_inner_;
  qi::rule<Iterator, std::string()> sq_;
  qi::rule<Iterator, std::string()> dq_;
  qi::rule<Iterator, std::string()> text_;
  qi::rule<Iterator, StringLiteral(), ascii::space_type> str_;
  qi::rule<Iterator, MethodCall(), ascii::space_type> methodcall_;
  qi::rule<Iterator, ObjectLiteral::member_pair(), ascii::space_type> member_;
  qi::rule<Iterator, ObjectLiteral(), ascii::space_type> object_;
  qi::rule<Iterator, FunctionLiteral(), ascii::space_type> function_;
  qi::rule<Iterator, Expression(), ascii::space_type> exp_;
};

}

#endif // _ExpParser_h_
