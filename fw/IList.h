// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _IList_h_
#define _IList_h_

#include "FWError.h"

#include <string>

namespace fw {

struct IList {
  IList(const std::string& name, const std::string& value = "", IList* left = NULL, IList* right = NULL);

  operator std::string() const { return "(IList " + name + ":" + value + ")"; }
  std::string Print() const;

  std::string DrawNode(const std::string& context) const;

  const std::string name;
  std::string value;
  IList* left;
  IList* right;
};

}

#endif // _IList_h_
