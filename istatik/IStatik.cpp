// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "IStatik.h"

#include <string>
using std::string;

using namespace istatik;

IStatik::IStatik(const string& compiler_name)
  : m_compiler_name(compiler_name) {
}

IStatik::~IStatik() {
  //endwin();
}

void IStatik::run() {
}
