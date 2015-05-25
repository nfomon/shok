// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_List_h_
#define _statik_List_h_

#include "SError.h"

#include <ostream>
#include <string>

namespace statik {

struct List {
  List(const std::string& name, const std::string& value = "");

  operator std::string() const { return "(List " + name + ":" + value + ")"; }
  std::string DrawNode(const std::string& context) const;

  const std::string name;
  std::string value;
  List* left;
  List* right;
};

std::ostream& operator<< (std::ostream& out, const List& node);

}

#endif // _statik_List_h_
