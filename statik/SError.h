// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _SError_h_
#define _SError_h_

/* Compiler framework errors */

#include "SLog.h"

#include <stdexcept>

namespace statik {

class SError : public std::runtime_error {
public:
  SError(const std::string& what) : std::runtime_error(what) {
    g_log.error() << what;
  }
};

}

#endif // _SError_h_
