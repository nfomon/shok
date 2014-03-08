// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Executor_h_
#define _Executor_h_

/* VM instruction execution dispatch */

#include "Instruction.h"
#include "SymbolTable.h"

#include "util/Log.h"

#include <stdexcept>

namespace vm {

class Executor {
public:
  Executor(Log& log);

  void exec(const std::string& line);

private:
  Log& m_log;
  SymbolTable m_symbolTable;
  Instruction* m_instruction;
};

}

#endif // _Executor_h_
