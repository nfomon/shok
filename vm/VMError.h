// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _VMError_h_
#define _VMError_h_

/* VM errors */

#include "util/Log.h"

#include <stdexcept>

namespace vm {

class VMError : public std::runtime_error {
public:
  VMError(const std::string& what) : std::runtime_error(what) {}
  VMError(Log& log, const std::string& what) : std::runtime_error(what) {
    log.error(what);
  }
};

}

#endif // _VMError_h_
