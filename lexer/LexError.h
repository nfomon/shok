// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _LexError_h_
#define _LexError_h_

/* Lexer errors */

#include "util/Log.h"

#include <stdexcept>

namespace lexer {

class LexError : public std::runtime_error {
public:
  LexError(const std::string& what) : std::runtime_error(what) {}
  LexError(Log& log, const std::string& what) : std::runtime_error(what) {
    log.error(what);
  }
};

}

#endif // _LexError_h_
