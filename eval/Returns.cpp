// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Returns.h"

#include "CompileError.h"
#include "TypeSpec.h"

#include <memory>
#include <string>
#include <vector>
using std::auto_ptr;
using std::string;
using std::vector;

using namespace compiler;

void Returns::setup() {
  if (children.size() != 1) {
    throw CompileError("TypeSpec" + print() + " must have 1 child");
  }
  m_typeSpec = dynamic_cast<TypeSpec*>(children.at(0));
  if (!m_typeSpec) {
    throw CompileError("Arg " + print() + " child must be a TypeSpec");
  }
}

string Returns::getName() const {
  if (!isSetup || !m_typeSpec) {
    throw CompileError("Cannot get name of Function-Returns "  + print() + " that has not been setup or is missing typespec");
  }
  return m_typeSpec->getTypeName();
}

auto_ptr<Type> Returns::getType() const {
  if (!isSetup || !m_typeSpec) {
    throw CompileError("Cannot get type of Function-Returns "  + print() + " that has not been setup or is missing typespec");
  }
  return m_typeSpec->getType();
}
