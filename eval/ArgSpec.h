// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _ArgSpec_h_
#define _ArgSpec_h_

/* The specification of a function argument */

#include "Log.h"
#include "Type.h"

#include <memory>
#include <string>

namespace eval {

class ArgSpec {
public:
  ArgSpec(const std::string& name,
          std::auto_ptr<Type> type)
          //Object* defaultValue,
          //bool optional = false)
    : m_name(name),
      m_type(type) {}

private:
  std::string m_name;
  std::auto_ptr<Type> m_type;
};

};

#endif // _ArgSpec_h_
