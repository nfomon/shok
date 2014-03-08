// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Symbol_h_
#define _Symbol_h_

/* Symbol */

#include "Object.h"

#include <memory>
#include <string>

namespace vm {

class Symbol {
public:
  Symbol(const std::string& name, const Object& source);

private:
  std::string m_name;
  std::auto_ptr<Object> m_value;
};

}

#endif // _Symbol_h_
