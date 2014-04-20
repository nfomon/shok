// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _CmdLine_h_
#define _CmdLine_h_

/* An instruction unit of the command line (could be an actual command-line or
 * a code block). */

#include "Expression.h"

#include "util/Log.h"

#include <boost/spirit/include/qi.hpp>
namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace ascii = spirit::ascii;

#include <string>
#include <vector>

namespace compiler {

class CmdLine {
public:
  CmdLine(Log& log)
    : m_log(log) {}

private:
  Log& m_log;
};

template <typename Iterator>
struct CmdLineParser : qi::grammar<Iterator, std::string(), ascii::space_type> {
private:
  Log& m_log;

public:
  CmdLineParser(Log& log)
    : CmdLineParser::base_type(cmdline_),
      m_log(log),
      exp_(log) {
    using qi::alnum;
    using qi::char_;
    using qi::lexeme;
    using qi::lit;
    using qi::no_skip;
    using qi::print;

    statement_ %= lit("...");
    expblock_ %= lit('{') > exp_ > lit('}');
    cmdchar_ %= no_skip[ char_ - lit('{') - lit(']') ];
    codeblock_ %= lit('{') > +statement_ > lit('}');
    cmd_ %= cmdchar_ > *(cmdchar_ | expblock_);
    cmdline_ %=
      lit("[")
      > (cmd_ | codeblock_)
      > lit("]")
    ;
  }

private:
  ExpParser<Iterator> exp_;
  qi::rule<Iterator, std::string(), ascii::space_type> statement_;
  qi::rule<Iterator, std::string(), ascii::space_type> expblock_;
  qi::rule<Iterator, std::string(), ascii::space_type> cmdchar_;
  qi::rule<Iterator, std::string(), ascii::space_type> codeblock_;
  qi::rule<Iterator, std::string(), ascii::space_type> cmd_;
  qi::rule<Iterator, std::string(), ascii::space_type> cmdline_;
};

}

#endif // _CmdLine_h_
