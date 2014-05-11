// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Code_h_
#define _Code_h_

/* Code block */

#include "Common.h"
#include "NewInit.h"
#include "Scope.h"

#include <boost/bind.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
namespace phoenix = boost::phoenix;
namespace spirit = boost::spirit;
namespace ascii = spirit::ascii;
namespace qi = spirit::qi;

#include <string>
#include <vector>

namespace compiler {

/* The CodeParser is provided a scope for the block.  This is so that the
 * Compiler can own the global scope (code-blocks just "swap" into it), and
 * also so that Functions can own their scopes.  Those are the places a
 * CodeParser is used.  Nested code-blocks are handled by the CodeParser
 * itself, so those scopes are managed here within the CodeParser.
 */

template <typename Iterator>
struct CodeParser : qi::grammar<Iterator, std::string(Scope&), ascii::space_type> {
public:
  CodeParser(ExpParser<Iterator>& exp_)
    : CodeParser::base_type(code_, "code parser"),
      newinit_(exp_) {
    using phoenix::ref;
    using qi::_1;
    using qi::_a;
    using qi::_r1;
    using qi::_val;
    using qi::eps;
    using qi::lit;

    new_.name("new");
    statement_.name("statement");
    code_.name("code");

    new_ = lit("(new")
      > +newinit_(_r1)[_val += phoenix::bind(&NewInit::bytecode_asNew, _1)]
      > lit(")");

    statement_ %= new_(_r1);

    block_ %=
      lit('{')[phoenix::bind(&Scope::reParent, _a, _r1)]
      > +(
        (statement_(_a) > -lit(';'))
        | block_(_a)
      )
      > lit('}');

    code_ %=
      lit('{')
      > +(
        (statement_(_r1) > -lit(';'))
        | block_(_r1)
      )
      > lit('}');
  }

private:
  NewInitParser<Iterator> newinit_;
  qi::rule<Iterator, std::string(Scope&), ascii::space_type> new_;
  qi::rule<Iterator, std::string(Scope&), ascii::space_type> statement_;
  qi::rule<Iterator, std::string(Scope&), qi::locals<Scope>, ascii::space_type> block_;
  qi::rule<Iterator, std::string(Scope&), ascii::space_type> code_;
};

}

#endif // _Code_h_
