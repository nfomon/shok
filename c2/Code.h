// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Code_h_
#define _Code_h_

/* Code block */

//#include "Expression.h"

#include "util/Log.h"

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
  CodeParser(Log& log)
    : CodeParser::base_type(code_, "code parser"),
      m_log(log) {
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

    init_ %= lit("(init");

    new_ %= lit("(new");

    statement_ %= new_;

    code_ = omit[
      lit('{')
      > +statement_
      > lit('}')
    ];
  }

private:
  Log& m_log;

  qi::rule<Iterator, std::string(), ascii::space_type> init_;
  qi::rule<Iterator, std::string(), ascii::space_type> new_;
  qi::rule<Iterator, std::string(), ascii::space_type> statement_;
  qi::rule<Iterator, std::string(), ascii::space_type> code_;
};

}

#endif // _Code_h_
