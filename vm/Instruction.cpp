// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Instruction.h"

#include "Instructions.h"
#include "VMError.h"

using namespace vm;

void Exec_Instruction::operator() (const New& n) const {
  n.exec(m_context);
}

void Exec_Instruction::operator() (const Del& del) const {
  del.exec(m_context);
}

void Exec_Instruction::operator() (const Call& call) const {
  call.exec(m_context);
}
