// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Object.h"

#include "NewInit.h"

#include "util/Util.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <string>
#include <utility>
using std::string;

using namespace compiler;

void Object::init(const Scope& scope) {
  m_scope.reset(new ObjectScope(*this, scope));
  m_type.reset(new BasicType(m_scope->findRoot("object")->duplicate(), "object"));
}

void Object::attach_new(const NewInit& newInit) {
  m_bytecode += newInit.bytecode_asMember() + "\n";
  m_type->addMember(newInit.name(), newInit.type().duplicate());
}

Scope& Object::scope() const {
  return *dynamic_cast<Scope*>(m_scope.get());
}

string Object::bytecode() const {
  return "(object" + m_bytecode + ")";
}
