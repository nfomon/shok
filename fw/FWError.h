// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _FWError_h_
#define _FWError_h_

/* Compiler framework errors */

#include "util/Log.h"

#include <stdexcept>

namespace fw {

class FWError : public std::runtime_error {
public:
  FWError(const std::string& what) : std::runtime_error(what) {}
  FWError(Log& log, const std::string& what) : std::runtime_error(what) {
    log.error(what);
  }
};

}

#endif // _FWError_h_
