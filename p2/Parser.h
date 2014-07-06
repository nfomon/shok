// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Parser_h_
#define _Parser_h_

/* shok parser */

#include <istream>
#include <ostream>

namespace parser {

class Parser {
public:
  Parser(std::istream& input, std::ostream& output);

  // returns true on full successful parse
  bool parse();

private:
  std::istream& m_input;
  std::ostream& m_output;
  unsigned long m_linenum;
};

}

#endif // _Parser_h_
