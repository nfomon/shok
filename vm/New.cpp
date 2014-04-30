// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "New.h"

#include "VMError.h"

#include "util/Util.h"

#include <string>
using std::auto_ptr;
using std::string;

// debug
#include <iostream>
using std::cout;
using std::endl;

using namespace vm;

void Exec_New::operator() (const New& n, qi::unused_type, qi::unused_type) const {
  cout << "New: name=" << n.name << endl;
  Exec_Exp exec_Exp(m_symbols);
  auto_ptr<Object> value = boost::apply_visitor(exec_Exp, n.exp);
  if (m_symbols.find(n.name) != m_symbols.end()) {
    throw VMError("Cannot insert symbol " + n.name + "; already exists");
  }
  m_symbols.insert(n.name, value);
}
