// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Cmd.h"

#include "VMError.h"

#include "util/Util.h"

#include <string>
#include <vector>
using std::auto_ptr;
using std::string;
using std::vector;

// debug
#include <iostream>
using std::cout;
using std::endl;

using namespace vm;

void Exec_Cmd::operator() (const Cmd& c, qi::unused_type, qi::unused_type) const {
  cout << "RUN CMD:" << c.cmd << endl;
  Exec_Exp exec_Exp(m_symbols);
  for (vector<Expression>::const_iterator i = c.exps.begin();
       i != c.exps.end(); ++i) {
    //auto_ptr<Object> exp = boost::apply_visitor(exec_Exp, c.exp);
    // The expression should have already provided us with the ->str.  So just
    // need to reach in to extract its actual string value.
  }
}
