// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _Object_h_
#define _Object_h_

/* Object */

#include "Log.h"
//#include "Type.h"

#include <string>

namespace eval {

class Object {
public:
  Object();
  //Object(const std::string& type);

  ~Object();

  //Object& getProperty() const;
  //std::auto_ptr<Object> call(const std::string& function, const Tuple& args);

private:
  //Type m_type;
};

};

#endif // _Object_h_
