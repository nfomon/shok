// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Compiler_h_
#define _Compiler_h_

/* shok compiler */

#include <boost/function.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/include/qi.hpp>

#include <istream>
#include <map>

namespace compiler {

class Compiler {
public:
  Compiler(std::istream& input);

  // returns true on full successful compile
  bool execute();

private:
  std::istream& m_input;

  // member-function aliases bound to their exec_x counterparts at construction
  boost::function<void (const std::string&, boost::spirit::qi::unused_type, boost::spirit::qi::unused_type)> on_cmdline;

  // semantic actions
  void emit(const std::string& bytecode);
};

}

#endif // _Compiler_h_
