// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Cmd_h_
#define _Cmd_h_

/* A program (and its arguments) to invoke, preceded by expressions that should
 * be interpolated into the command. */

#include "Context.h"
#include "Expression.h"

#include <boost/optional.hpp>
#include <boost/utility.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/variant.hpp>
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;

#include <string>
#include <vector>

// debug
#include <iostream>
using std::cout;
using std::endl;

namespace vm {

struct Cmd {
  typedef std::vector<Expression> exp_vec;
  typedef exp_vec::const_iterator exp_iter;

  exp_vec exps;
  std::string cmd;
};

struct Exec_Cmd {
public:
  Exec_Cmd(Context& context)
    : m_context(context) {}

  void operator() (const Cmd& c, qi::unused_type, qi::unused_type) const;

private:
  Context& m_context;
};

template <typename Iterator> struct ExpParser;

template <typename Iterator>
struct CmdParser : qi::grammar<Iterator, Cmd(), ascii::space_type> {
public:
  CmdParser(ExpParser<Iterator>& exp_)
    : CmdParser::base_type(cmd_, "Cmd"),
      exp_(exp_) {
    using qi::char_;
    using qi::int_;
    using qi::lit;
    using qi::no_skip;

    runcmd_ %= lit('[') > +no_skip[~char_("]")] > lit(']');
    cmd_ %=
      lit("(cmd")
      > *(lit("(exp") > exp_ > ")")
      > runcmd_
      > lit(')');

    //BOOST_SPIRIT_DEBUG_NODE(runcmd_);
    //BOOST_SPIRIT_DEBUG_NODE(cmd_);
  }

private:
  ExpParser<Iterator>& exp_;

  qi::rule<Iterator, std::string(), ascii::space_type> runcmd_;
  qi::rule<Iterator, Cmd(), ascii::space_type> cmd_;
};

}

#endif // _Cmd_h_
