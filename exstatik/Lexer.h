// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _exstatik_Lexer_h_
#define _exstatik_Lexer_h_

#include "statik/Rule.h"

#include <memory>

namespace exstatik {

std::auto_ptr<statik::Rule> CreateLexer_Simple();
std::auto_ptr<statik::Rule> CreateLexer_Splash();
std::auto_ptr<statik::Rule> CreateLexer_KeywordTest();
std::auto_ptr<statik::Rule> CreateLexer_Moderate();
std::auto_ptr<statik::Rule> CreateLexer_Complex();
std::auto_ptr<statik::Rule> CreateLexer_Nifty();

}

#endif // _exstatik_Lexer_h_
