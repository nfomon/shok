// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _New_h_
#define _New_h_

/* A New instruction: defines a symbol (name, type, value) */

#include "Instruction.h"
#include "SymbolTable.h"

#include "util/Log.h"

#include <stdexcept>

namespace vm {

class New : public Instruction {
public:
  New(Log& log)
    : Instruction(log),
      m_membertime(false),
      m_membersLeft(0) {}
  virtual ~New() {}

  virtual void insert(const std::string& line);
  virtual void execute(SymbolTable&);

protected:
  std::string m_name;
  std::string m_source;
  bool m_membertime;    // true when we are parsing members
  int m_membersLeft;
};

}

#endif // _New_h_
