// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Char_h_
#define _Char_h_

#include "OData.h"

#include <string>

namespace fw {

class CharData : public OData {
public:
  CharData(const char c)
    : c(c) {}
  virtual ~CharData() {}
  const char c;

  virtual operator std::string() const { return std::string("[CharData '") + c; }
};

}

#endif // _Char_h_
