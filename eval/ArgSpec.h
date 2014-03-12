// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _ArgSpec_h_
#define _ArgSpec_h_

/* The specification of a function argument */

#include "Type.h"

#include "util/Log.h"

#include <memory>
#include <string>

namespace eval {

class ArgSpec {
public:
  ArgSpec(const std::string& name,
          Type& type)
          //Object* defaultValue,
          //bool optional = false)
    : m_name(name),
      m_type(type) {}

  std::auto_ptr<ArgSpec> duplicate() const {
    return std::auto_ptr<ArgSpec>(new ArgSpec(m_name, *m_type.duplicate().release()));
  }

  const Type& type() const {
    return m_type;
  }

  std::string print() const {
    return "(argspec:" + m_name + ":" + m_type.print() + ")";
  }

private:
  std::string m_name;
  Type& m_type;
};

}

#endif // _ArgSpec_h_
