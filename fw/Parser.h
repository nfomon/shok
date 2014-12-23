// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Parser_h_
#define _Parser_h_

#include "Rule.h"

#include "util/Log.h"

#include <memory>

namespace fw {

std::auto_ptr<Rule> CreateParser_Simple(Log& log);
std::auto_ptr<Rule> CreateParser_Moderate(Log& log);
std::auto_ptr<Rule> CreateParser_Complex(Log& log);

}

#endif // _Parser_h_
