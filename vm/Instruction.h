// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Instruction_h_
#define _Instruction_h_

/* A single VM instruction. */

#include "SymbolTable.h"

#include "util/Log.h"

#include <stdexcept>

namespace vm {

class Instruction {
public:
  static Instruction* MakeInstruction(Log& log, const std::string& name);

  Instruction(Log& log)
    : m_log(log) {}
  virtual ~Instruction() {}

  // Insert a line of input.  Some instructions own multiple lines, so they
  // should be inserted until isDone() is true.
  virtual void insert(const std::string& line) = 0;
  // execute the instruction: should be called once isDone() is true
  virtual void execute(SymbolTable&) = 0;

  // true when all the lines we expect have been inserted
  bool isDone() const { return m_done; }

protected:
  Log& m_log;
  bool m_done;
};

}

#endif // _Instruction_h_
