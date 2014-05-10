// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Code_h_
#define _Code_h_

/* Code block */

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

template <typename Iterator>
struct CodeParser : qi::grammar<Iterator, std::string(), ascii::space_type> {
public:
  CodeParser(Scope& globalScope)
    : CodeParser::base_type(code_, "code parser"),
      m_globalScope(globalScope) {
    using phoenix::ref;
    using qi::_1;
    using qi::_val;
    using qi::lit;

    new_.name("new");
    statement_.name("statement");
    code_.name("code");

    new_ = lit("(new")
      > +newinit_(ref(m_globalScope))[_val += phoenix::bind(&NewInit::bytecode_asNew, _1)]
      > lit(")");

    // A code block would construct a new block that gets passed to *its* child
    // statements btw

    statement_ %= new_;
    code_ %=
      lit('{')
      > +(statement_ > -lit(';'))
      > lit('}');
  }

private:
  Scope& m_globalScope;

  NewInitParser<Iterator> newinit_;
  qi::rule<Iterator, std::string(), ascii::space_type> new_;
  qi::rule<Iterator, std::string(), ascii::space_type> statement_;
  qi::rule<Iterator, std::string(), ascii::space_type> code_;
};

}

#endif // _Code_h_
