// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _StatikError_h_
#define _StatikError_h_

#include "StatikLog.h"

#include <stdexcept>

namespace statik {

class StatikError : public std::runtime_error {
public:
  StatikError(const std::string& what) : std::runtime_error(what) {
    g_log.error() << what;
  }
};

}

#endif // _StatikError_h_
