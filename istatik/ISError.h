// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _istatik_ISError_h_
#define _istatik_ISError_h_

#include "ISLog.h"

#include <stdexcept>

namespace istatik {

class ISError : public std::runtime_error {
public:
  ISError(const std::string& what) : std::runtime_error(what) {
    g_log.error() << what;
  }
};

}

#endif // _istatik_ISError_h_
