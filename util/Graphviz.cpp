// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Graphviz.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <string>
using std::string;

using namespace Util;

string Util::dotVar(const void* x, const std::string& context) {
  return context + "_" + lexical_cast<string>(x);
}
