// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Parser_h_
#define _Parser_h_

#include "Rule.h"

#include <memory>

namespace fw {

std::auto_ptr<Rule> CreateParser_Simple();
std::auto_ptr<Rule> CreateParser_Moderate();
std::auto_ptr<Rule> CreateParser_Complex();
std::auto_ptr<Rule> CreateParser_Nifty();

}

#endif // _Parser_h_
