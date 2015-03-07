// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "IList.h"

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

IList::IList(const string& name, const string& value)
  : name(name),
    value(value),
    left(NULL),
    right(NULL) {
}

string IList::Print() const {
  string s = "<" + string(*this) + ">";
  if (right) {
    s += "-" + right->Print();
  }
  return s;
}

string IList::DrawNode(const string& context) const {
  g_log.debug() << "Drawing List node " << *this << " with " << (left ? "left" : "no left") << " and " << (right ? "right" : "no right");
  string s = dotVar(this, context) + " [label=\"" + safeLabelStr(name + (value.empty() ? "" : (name.empty() ? "" : ":") + value)) + "\", style=\"filled\", fillcolor=\"#dddddd\", fontsize=12.0];\n";
  g_log.debug() << "Drawing node " << *this << (right ? "+" : "");
  if (right) {
    s += dotVar(this, context) + " -> " + dotVar(right, context) + ";\n";
    s += right->DrawNode(context);
  }
  return s;
}

/* non-member */

ostream& statik::operator<< (ostream& out, const IList& node) {
  out << string(node);
  return out;
}
