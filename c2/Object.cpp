// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Object.h"

#include "NewInit.h"

#include <string>
#include <utility>
using std::string;

using namespace compiler;

void Object::init(const Scope& scope) {
  m_scope.reset(new ObjectScope(scope, *this));
  m_type.reset(new BasicType(m_scope->findRoot("object")->duplicate(), "object"));
}

void Object::attach_new(const NewInit& newInit) {
  m_bytecode += newInit.bytecode_asMember();
  m_type->addMember(newInit.name(), newInit.type().duplicate());
}

ObjectScope& Object::scope() const {
  return *m_scope;
}

const Type& Object::type() const {
  if (!m_type.get()) {
    throw CompileError("Object " + bytecode() + " does not have a type");
  }
  return *m_type;
}

string Object::bytecode() const {
  return " (object" + m_bytecode + ")";
}
