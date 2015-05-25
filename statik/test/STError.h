// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_test_STError_h_
#define _statik_test_STError_h_

#include "STLog.h"

#include <stdexcept>

namespace statik_test {

class STError : public std::runtime_error {
public:
  STError(const std::string& what) : std::runtime_error(what) {
    g_log.error() << what;
  }
};

}

#endif // _statik_test_STError_h_
