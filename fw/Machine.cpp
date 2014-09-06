// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Machine.h"
#include "State.h"

using namespace fw;

std::auto_ptr<State> MachineNode::MakeState() const {
  return std::auto_ptr<State>(new MachineState(*this));
}
