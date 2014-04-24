// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Code_h_
#define _Code_h_

/* Code block */

#include "NewInit.h"
#include "Scope.h"

#include <boost/bind.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
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
    using phoenix::val;
    using qi::_val;
    using qi::alnum;
    using qi::char_;
    using qi::lexeme;
    using qi::lit;
    using qi::omit;
    using qi::print;

    code_.name("code");

    new_ %= lit("(new") > +newinit_(ref(m_globalScope)) > lit(")");

    statement_ %= new_;
    code_ %=
      lit('{')
      > +statement_
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
