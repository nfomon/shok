// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Lexer_h_
#define _Lexer_h_

/* Lexical analyzer */

#include "Common.h"
#include "Emitter.h"

#include "util/Log.h"

#include <istream>
#include <ostream>

namespace lexer {

class Lexer {
public:
  Lexer(Log& log, std::istream& input, std::ostream& output);

  // returns true on full successful lex
  bool lex();

private:
  Log& m_log;
  std::istream& m_input;
  std::ostream& m_output;
  linenum_t m_linenum;
  charnum_t m_charnum;
  Emitter m_emitter;
};

}

#endif // _Lexer_h_
