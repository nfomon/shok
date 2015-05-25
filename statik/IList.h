// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_IList_h_
#define _statik_IList_h_

#include "SError.h"

#include <ostream>
#include <string>

namespace statik {

struct IList {
  IList(const std::string& name, const std::string& value = "");

  operator std::string() const { return "(IList " + name + ":" + value + ")"; }
  std::string DrawNode(const std::string& context) const;

  const std::string name;
  std::string value;
  IList* left;
  IList* right;
};

std::ostream& operator<< (std::ostream& out, const IList& node);

}

#endif // _statik_IList_h_
