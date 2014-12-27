// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "IList.h"

#include "FWTree.h"

#include "util/Graphviz.h"
using Util::dotVar;
using Util::safeLabelStr;

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace fw;

IList::IList(const std::string& name, const std::string& value, IList* left, IList* right)
  : name(name),
    value(value),
    left(left),
    right(right) {
}

string IList::Print() const {
  string s = "<" + string(*this) + ">";
  if (right) {
    s += "-" + right->Print();
  }
  return s;
}

string IList::DrawNode(const string& context) const {
  string s;
  s = dotVar(this, context) + " [label=\"" + safeLabelStr(name + (value.empty() ? "" : (name.empty() ? "" : ":") + value)) + "\", style=\"filled\", fillcolor=\"#dddddd\", fontsize=12.0];\n";
  if (right) {
    s += dotVar(this, context) + " -> " + dotVar(right, context) + ";\n";
    s += right->DrawNode(context);
  }
  return s;
}
