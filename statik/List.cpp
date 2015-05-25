// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "List.h"

#include "SLog.h"
#include "STree.h"

#include "util/Graphviz.h"
using Util::dotVar;
using Util::safeLabelStr;

#include <memory>
#include <ostream>
#include <string>
using std::auto_ptr;
using std::ostream;
using std::string;

using namespace statik;

/* public */

List::List(const string& name, const string& value)
  : name(name),
    value(value),
    left(NULL),
    right(NULL) {
}

string List::DrawNode(const string& context) const {
  string s = dotVar(this, context) + " [label=\"" + safeLabelStr(name + (value.empty() ? "" : (name.empty() ? "" : ":") + value)) + "\", style=\"filled\", fillcolor=\"#dddddd\", fontsize=12.0];\n";
  if (right) {
    s += dotVar(this, context) + " -> " + dotVar(right, context) + ";\n";
    s += right->DrawNode(context);
  }
  return s;
}

/* non-member */

ostream& statik::operator<< (ostream& out, const List& node) {
  out << string(node);
  return out;
}
