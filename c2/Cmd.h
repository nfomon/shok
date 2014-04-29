// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Cmd_h_
#define _Cmd_h_

/* A command-line with an actual program to invoke (not a code block). */

#include "Expression.h"
#include "Scope.h"

#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/qi.hpp>
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;

#include <string>
#include <vector>

namespace compiler {

template <typename Iterator>
struct CmdParser : qi::grammar<Iterator, std::string(), ascii::space_type> {
public:
  CmdParser(Scope& globalScope)
    : CmdParser::base_type(cmd_, "command parser"),
      m_globalScope(globalScope) {
    using qi::_val;
    using qi::char_;
    using qi::lit;
    using qi::no_skip;
    using qi::omit;
    using phoenix::ref;

    // Write expression bytecode before the cmd.
    // The cmd uses "{}" as placeholders for the expression's computed result.

    expblock_ %= (
      lit('{')
      > omit[ exp_(ref(m_globalScope))[ref(m_expcode) += phoenix::bind(&Expression::bytecode, qi::_1)] ]
      > lit('}')
    )[_val += "{}"];
    cmdtext_ %= +no_skip[ char_ - lit('{') - lit(']') ];
    cmdwhole_ %= (cmdtext_ > *(expblock_ > -cmdtext_));
    cmd_ %= cmdwhole_[_val = ref(m_expcode) + qi::_1];
  }

private:
  std::string m_expcode;
  Scope& m_globalScope;

  ExpParser<Iterator> exp_;
  qi::rule<Iterator, std::string(), ascii::space_type> expblock_;
  qi::rule<Iterator, std::string(), ascii::space_type> cmdtext_;
  qi::rule<Iterator, std::string(), ascii::space_type> cmdwhole_;
  qi::rule<Iterator, std::string(), ascii::space_type> cmd_;
};

}

#endif // _Cmd_h_
