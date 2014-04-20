// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Compiler_h_
#define _Compiler_h_

/* shok compiler */

//#include "AST.h"

#include "util/Log.h"

#include <boost/function.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/include/qi.hpp>

#include <istream>
#include <map>

namespace compiler {

class Compiler {
public:
  Compiler(Log& log, std::istream& input);

  // returns true on full successful compile
  bool execute();

private:
  Log& m_log;
  std::istream& m_input;
  //AST m_ast;

  // member-function aliases bound to their exec_x counterparts at construction
  boost::function<void (const std::string&, boost::spirit::qi::unused_type, boost::spirit::qi::unused_type)> on_cmdline;

  // semantic actions
  void exec_cmdline(const std::string& cmdline);
};

}

#endif // _Compiler_h_
