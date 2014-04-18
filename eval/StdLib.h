// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _StdLib_h_
#define _StdLib_h_

/* shok standard library */

#include "Object.h"
#include "Scope.h"

#include "util/Log.h"

#include <vector>

namespace compiler {

class StdLib {
public:
  StdLib(Log&, Scope& rootScope);
  ~StdLib();

private:
  typedef std::vector<Object*> stdlib_vec;
  typedef stdlib_vec::const_iterator stdlib_iter;

  Log& m_log;
  Scope& m_scope;
  stdlib_vec m_objects;
};

}

#endif // _StdLib_h_
