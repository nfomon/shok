// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Del.h"

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

void Exec_Del::operator() (const std::string& name, qi::unused_type, qi::unused_type) const {
  cout << "Del: name=" << name << endl;
  symbol_mod_iter s = m_symbols.find(name);
  if (m_symbols.end() == s) {
    throw VMError("No such symbol " + name + " to delete");
  }
  m_symbols.erase(s);
}
