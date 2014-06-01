// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Cmd.h"

#include "Instructions.h"
#include "VMError.h"

#include <boost/variant/get.hpp>

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
  string cmd = c.cmd;
  Exec_Exp exec_Exp(m_context);
  size_t braces_pos = 0;
  for (Cmd::exp_iter i = c.exps.begin(); i != c.exps.end(); ++i) {
    auto_ptr<Object> exp = boost::apply_visitor(exec_Exp, *i);
    // The expression should have already provided us with the ->str.  So just
    // need to reach in to extract its actual string value.
    boost::optional<BuiltinData> builtin = exp->builtinData();
    if (!builtin) {
      throw VMError("Cmd expression was not a string");
    }
    const string& exp_str = boost::get<std::string>(*builtin);
    braces_pos = cmd.find("{}", braces_pos);
    if (string::npos == braces_pos) {
      throw VMError("Too many args provided to cmd " + c.cmd);
    }
    cmd.erase(braces_pos, 2);
    cmd.insert(braces_pos, exp_str);
    braces_pos += exp_str.size();
  }
  if (cmd.find("{}", braces_pos) != string::npos) {
    throw VMError("Not enough args provided to cmd " + c.cmd);
  }
  cout << "RUN:" << cmd << endl;
}
