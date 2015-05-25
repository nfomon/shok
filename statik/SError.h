// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_SError_h_
#define _statik_SError_h_

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

#endif // _statik_SError_h_
