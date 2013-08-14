// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _stdlib_object_h_
#define _stdlib_object_h_

/* object */

#include <string>
#include <vector>

namespace eval {
namespace stdlib {

class object : public Object {
public:
  object(Log& log)
    : Object(log, "object") {
    m_type.reset(new NullType());
  }

private:
};

};
};

#endif // _stdlib_object_h_
