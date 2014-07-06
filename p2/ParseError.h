// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _ParseError_h_
#define _ParseError_h_

/* Parse errors */

#include "util/Log.h"

#include <stdexcept>

namespace parser {

class ParseError : public std::runtime_error {
public:
  ParseError(const std::string& what) : std::runtime_error(what) {}
  ParseError(Log& log, const std::string& what) : std::runtime_error(what) {
    log.error(what);
  }
};

}

#endif // _ParseError_h_
