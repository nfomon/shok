// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _IList_h_
#define _IList_h_

#include "SError.h"

#include <ostream>
#include <string>

namespace statik {

struct IList {
  IList(const std::string& name, const std::string& value = "");

  operator std::string() const { return "(IList " + name + ":" + value + ")"; }
  std::string Print() const;
  std::string DrawNode(const std::string& context) const;

  const std::string name;
  std::string value;
  IList* left;
  IList* right;
  bool isDeleted;
};

std::ostream& operator<< (std::ostream& out, const IList& node);

}

#endif // _IList_h_
