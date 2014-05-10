// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Cmd_h_
#define _Cmd_h_

/* A command-line with an actual program to invoke (not a code block). */

#include "Expression.h"
#include "NewInit.h"
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

class Cmd {
public:
  void attach_text(const std::string& text);
  void attach_exp(const Expression& exp);
  std::string bytecode() const;

private:
  std::string m_expcode;
  std::string m_cmdtext;
};

template <typename Iterator>
struct CmdParser : qi::grammar<Iterator, std::string(), qi::locals<Cmd>, ascii::space_type> {
public:
  CmdParser(Scope& globalScope)
    : CmdParser::base_type(cmd_, "command parser"),
      m_globalScope(globalScope),
      exp_(newinit_) {
    using qi::_1;
    using qi::_a;
    using qi::_r1;
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
      > exp_(ref(m_globalScope))[phoenix::bind(&Cmd::attach_exp, _r1, _1)]
      > lit('}')
    );

    cmdchars_ %= (+(~char_("{]")));
    cmdtext_ = cmdchars_[phoenix::bind(&Cmd::attach_text, _r1, _1)];
    cmdwhole_ = cmdtext_(_r1) > *(expblock_(_r1)) > -cmdtext_(_r1);
    cmd_ = cmdwhole_(_a)[_val = phoenix::bind(&Cmd::bytecode, _a)];
  }

private:
  Scope& m_globalScope;
  // This must be provided to exp_ to break a circular-dependency
  NewInitParser<Iterator> newinit_;

  ExpParser<Iterator> exp_;
  qi::rule<Iterator, void(Cmd&), ascii::space_type> expblock_;
  qi::rule<Iterator, std::string()> cmdchars_;
  qi::rule<Iterator, void(Cmd&)> cmdtext_;
  qi::rule<Iterator, void(Cmd&), ascii::space_type> cmdwhole_;
  qi::rule<Iterator, std::string(), qi::locals<Cmd>, ascii::space_type> cmd_;
};

}

#endif // _Cmd_h_
