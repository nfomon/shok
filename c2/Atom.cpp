// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Atom.h"

#include "Function.h"
#include "Object.h"
#include "Variable.h"

#include "util/Util.h"

#include <string>
#include <utility>
using std::string;

using namespace compiler;

std::string Atom_bytecode::operator() (const Variable& var) const {
  return " " + var.bytename();
}

std::string Atom_bytecode::operator() (const Object& object) const {
  return object.bytecode();
}

std::string Atom_bytecode::operator() (const Function& function) const {
  return function.bytecode();
}

const Type& Atom_type::operator() (const Variable& var) const {
  return var.type();
}

const Type& Atom_type::operator() (const Object& object) const {
  return object.type();
}

const Type& Atom_type::operator() (const Function& function) const {
  return function.type();
}
