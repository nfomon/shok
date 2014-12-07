// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _OData_h_
#define _OData_h_

/* Data on an output list node that can be of interest to a subsequent
 * connection. */

#include <string>

namespace fw {

struct OData {
  virtual ~OData() {}
  virtual operator std::string() const { return "(Data:empty)"; }
};

}

#endif // _OData_h_
