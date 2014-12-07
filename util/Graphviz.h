// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Graphviz_h_
#define _Graphviz_h_

/* Handy functions for interfacing with graphviz */

#include <string>

namespace Util {
  std::string dotVar(const void*, const std::string& context);
}

#endif // _Graphviz_h_
