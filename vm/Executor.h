// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Executor_h_
#define _Executor_h_

/* VM instruction execution dispatch */

#include "Object.h"
#include "SymbolTable.h"

#include "util/Log.h"

#include <boost/function.hpp>
#include <boost/spirit/home/support/unused.hpp>
#include <boost/spirit/include/qi.hpp>

#include <istream>
#include <map>

namespace vm {

class New;

class Executor {
public:
  Executor(Log& log, std::istream& input);

  // returns true on full successful parse
  bool execute();

private:
  Log& m_log;
  std::istream& m_input;
  symbol_map m_symbols;

  // semantic actions
  void exec_new(const New&);
};

}

#endif // _Executor_h_
