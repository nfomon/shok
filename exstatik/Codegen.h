// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Codegen_h_
#define _Codegen_h_

#include "statik/Rule.h"

#include <memory>

namespace exstatik {

/*
std::auto_ptr<statik::Rule> CreateCodegen_Simple();
std::auto_ptr<statik::Rule> CreateCodegen_Moderate();
std::auto_ptr<statik::Rule> CreateCodegen_Complex();
*/
std::auto_ptr<statik::Rule> CreateCodegen_Nifty();

}

#endif // _Codegen_h_
