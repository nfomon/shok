// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Rule.h"

#include "State.h"

#include "util/Graphviz.h"
using Util::dotVar;

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace fw;

auto_ptr<State> Rule::MakeState() const {
  return auto_ptr<State>(new State(*this));
}

string Rule::print() const {
  string s(m_name);
  if (!m_children.empty()) {
    s += " (";
    for (child_iter i = m_children.begin(); i != m_children.end(); ++i) {
      if (i != m_children.begin()) { s += ", "; }
      s += i->print();
    }
    s += ")";
  }
  return s;
}

string Rule::DrawNode(const std::string& context) const {
  string s;
  s += dotVar(this, context) + " [label=\"" + m_name + "\"];\n";
  for (child_iter i = m_children.begin(); i != m_children.end(); ++i) {
    s += dotVar(this, context) + " -> " + dotVar(&*i, context) + ";\n";
    s += i->DrawNode(context);
  }
  return s;
}
