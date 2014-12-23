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

IList::IList(auto_ptr<OData> data, IList* left, IList* right)
  : owner(NULL),
    left(left),
    right(right) {
  m_data = data;
}

IList::IList(const FWTree* owner, IList* left, IList* right)
  : owner(owner),
    left(left),
    right(right) {
  if (owner) {
    m_data = owner->GetState().rule.MakeData(*owner);
  }
}

string IList::DrawNode(const string& context) const {
  string s;
  s = dotVar(this, context) + " [label=\"" + safeLabelStr(string(GetData())) + "\", style=\"filled\", fillcolor=\"#dddddd\", fontsize=12.0];\n";
  if (right) {
    s += dotVar(this, context) + " -> " + dotVar(right, context) + ";\n";
    s += right->DrawNode(context);
  }
  return s;
}
