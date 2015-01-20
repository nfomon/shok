// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Compiler_h_
#define _Compiler_h_

#include "statik/Rule.h"

#include <memory>

namespace exstatik {

/*
std::auto_ptr<statik::Rule> CreateCompiler_Simple();
std::auto_ptr<statik::Rule> CreateCompiler_Moderate();
std::auto_ptr<statik::Rule> CreateCompiler_Complex();
*/
std::auto_ptr<statik::Rule> CreateCompiler_Nifty();

}

#endif // _Compiler_h_
