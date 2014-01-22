// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "StdLib.h"

#include "EvalError.h"
#include "Log.h"

#include <memory>
#include <vector>
using std::auto_ptr;
using std::vector;

using namespace eval;

/* public */

StdLib::StdLib(Log& log, Scope& rootScope)
  : m_log(log), m_scope(rootScope) {

  // object: the root of the object tree
  auto_ptr<Object> objectObject(new Object(m_log, "object"));
  auto_ptr<Type> objectType(new RootType(m_log));
  Symbol& object = m_scope.newSymbol("object", objectType);
  //objectType->addMemberType("foo", auto_ptr<Type>(new BasicType(m_log, objectSymbol)));
  //object.newMember("foo", auto_ptr<Object>(new Object(m_log, "foo")));
  m_scope.initSymbol("object", objectObject);

  auto_ptr<Object> functionObject(new Object(m_log, "@"));
  auto_ptr<Type> functionType(new BasicType(m_log, object));
  //Symbol& function = m_scope.newSymbol("@", functionType);
  m_scope.newSymbol("@", functionType);
  m_scope.initSymbol("@", functionObject);

  m_scope.commitAll();
}

// Nothing to do.  m_objects objects are owned by m_scope.
StdLib::~StdLib() {
}
