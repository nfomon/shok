// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _StdLib_h_
#define _StdLib_h_

/* shok standard library */

#include "Scope.h"
#include "Type.h"

#include <memory>
#include <string>

namespace compiler {

namespace StdLib {

const std::string OBJECT = "object";
const std::string FUNCTION = "@";
const std::string INTEGER = "int";
const std::string STRING = "str";

void Init(Scope& scope);

}

}

#endif // _StdLib_h_
