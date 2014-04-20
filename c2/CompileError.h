// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _CompileError_h_
#define _CompileError_h_

/* Compilation errors */

#include "util/Log.h"

#include <stdexcept>

namespace compiler {

class CompileError : public std::runtime_error {
public:
  CompileError(const std::string& what) : std::runtime_error(what) {}
  CompileError(Log& log, const std::string& what) : std::runtime_error(what) {
    log.error(what);
  }
};

}

#endif // _CompileError_h_
