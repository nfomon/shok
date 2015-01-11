// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Lexer_h_
#define _Lexer_h_

#include "Rule.h"

#include <memory>

namespace fw {

std::auto_ptr<Rule> CreateLexer_Simple();
std::auto_ptr<Rule> CreateLexer_Moderate();
std::auto_ptr<Rule> CreateLexer_Complex();
std::auto_ptr<Rule> CreateLexer_Nifty();

}

#endif // _Lexer_h_
