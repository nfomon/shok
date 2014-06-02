// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Compiler_h_
#define _Compiler_h_

/* shok compiler */

#include <istream>
#include <ostream>

namespace compiler {

class Compiler {
public:
  Compiler(std::istream& input, std::ostream& output);

  // returns true on full successful compile
  bool execute();

private:
  std::istream& m_input;
  std::ostream& m_output;
};

}

#endif // _Compiler_h_
