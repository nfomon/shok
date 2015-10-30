// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _exstatik_Parser_h_
#define _exstatik_Parser_h_

#include "statik/Rule.h"

#include <memory>

namespace exstatik {

std::auto_ptr<statik::Rule> CreateParser_Simple();
std::auto_ptr<statik::Rule> CreateParser_Moderate();
std::auto_ptr<statik::Rule> CreateParser_Complex();
std::auto_ptr<statik::Rule> CreateParser_Nifty();
std::auto_ptr<statik::Rule> CreateParser_JSON();
std::auto_ptr<statik::Rule> CreateParser_C();

}

#endif // _exstatik_Parser_h_
