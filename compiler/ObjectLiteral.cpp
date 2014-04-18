// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "ObjectLiteral.h"

#include "boost/lexical_cast.hpp"

#include "Args.h"
#include "CompileError.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace compiler;

void ObjectLiteral::setup() {
  if (children.size() > 1) {
    throw CompileError("ObjectLiteral " + print() + " must have <= 1 children");
  }
  if (1 == children.size()) {
    m_body = dynamic_cast<Block*>(children.at(0));
    if (!m_body) {
      throw CompileError("ObjectLiteral " + print() + "'s only child must be a Block");
    }
    m_inits = m_body->getInits();
  }
  // Don't defer; allow our child NewInits to commit to our scope
  //m_body->defer();
  computeType();

  // TODO we're doing our block's NewInit prepare() too late for ASAP I think;
  // the ObjectLiteral should be analyzing each member as they are inserted.
  for (init_iter i = m_inits.begin(); i != m_inits.end(); ++i) {
    (*i)->prepare();
  }
}

void ObjectLiteral::compile() {
}

auto_ptr<Object> ObjectLiteral::makeObject(const string& newName) const {
  auto_ptr<Object> o(new Object(log, newName));
  for (init_iter i = m_inits.begin(); i != m_inits.end(); ++i) {
    o->newMember((*i)->getName(), (*i)->getObject());
  }
  log.debug("ObjectLiteral " + print() + " made an Object with " + boost::lexical_cast<string>(m_inits.size()) + " members");
  return o;
}

void ObjectLiteral::computeType() {
  if (m_type.get()) {
    throw CompileError("Cannot compute type of ObjectLiteral  " + print() + " that already has a type");
  }
  const Symbol* object = parentScope->getSymbol("object");
  if (!object) {
    throw CompileError("Cannot find symbol for the object object");
  }
  log.debug("Computing type of ObjectLiteral " + print());
  BasicType* type = new BasicType(log, *object);
  m_type.reset(type);
  for (init_iter i = m_inits.begin(); i != m_inits.end(); ++i) {
    log.debug(" - adding member " + (*i)->print() + " to ObjectLiteral " + print() + "'s type");
    type->addMemberType((*i)->getName(), (*i)->getType());
  }
}
