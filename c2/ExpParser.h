// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _ExpParser_h_
#define _ExpParser_h_

/* Expression */

#include "Expression.h"
#include "Function.h"
#include "Literal.h"
#include "Object.h"
#include "Scope.h"
#include "Variable.h"

#include <boost/fusion/include/std_pair.hpp>
#include <boost/optional.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
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

#include <string>

namespace compiler {

/* Second qi inherited attribute (std::string): should be "type" to match a
 * type specifier, otherwise "exp" to match a plain expression. */
template <typename Iterator>
struct ExpParser : qi::grammar<Iterator, Expression(const Scope&, std::string), ascii::space_type> {
public:
  ExpParser()
    : ExpParser::base_type(exp_, "Expression"),
      object_(*this),
      function_(*this) {
    using ascii::string;
    using phoenix::ref;
    using qi::_1;
    using qi::_a;
    using qi::_r1;
    using qi::_r2;
    using qi::_val;
    using qi::lit;

    atom_ = (
      variable_(_r1)
      | literal_(_r1)
      | object_(_r1)
      | function_(_r1)
    )[phoenix::bind(&Expression::attach_atom, _r2, _1)];
    preop_ %= (
      string("PLUS") | string("MINUS")
    )[phoenix::bind(&Expression::attach_preop, _r1, _1)];
    binop_ %= (
      string("PLUS") | string("MINUS") | string("STAR") | string("SLASH")
    )[phoenix::bind(&Expression::attach_binop, _r1, _1)];

    exp_ = (
      lit("(")
      >> ascii::string(_r2)[phoenix::bind(&Expression::init, _val, _r1)]
      > -preop_(_val)
      > atom_(_r1, _val)
      > *(binop_(_val) > -preop_(_val) > atom_(_r1, _val))
      > lit(")")[phoenix::bind(&Expression::finalize, _val)]
    );

    //BOOST_SPIRIT_DEBUG_NODE(atom_);
    //BOOST_SPIRIT_DEBUG_NODE(preop_);
    //BOOST_SPIRIT_DEBUG_NODE(binop_);
    //BOOST_SPIRIT_DEBUG_NODE(exp_);
  }

private:
  VariableParser<Iterator> variable_;
  LiteralParser<Iterator> literal_;
  ObjectParser<Iterator> object_;
  FunctionParser<Iterator> function_;
  qi::rule<Iterator, void(const Scope&, Expression&), ascii::space_type> atom_;
  qi::rule<Iterator, void(Expression&), ascii::space_type> preop_;
  qi::rule<Iterator, void(Expression&), ascii::space_type> binop_;
  qi::rule<Iterator, Expression(const Scope&, std::string), ascii::space_type> exp_;
};

}

#endif // _ExpParser_h_
