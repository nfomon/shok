// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Instruction.h"

#include "New.h"

#include "VMError.h"

#include <string>
using std::string;

using namespace vm;

Instruction* Instruction::MakeInstruction(Log& log, const string& name) {
  if ("new" == name) {
    return new New(log);
  }
  throw VMError("Unrecognized instruction (" + name + ")");
}
