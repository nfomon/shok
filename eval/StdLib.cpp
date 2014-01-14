// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "StdLib.h"

#include "Log.h"
#include "EvalError.h"

#include <memory>
#include <vector>
using std::auto_ptr;
using std::vector;

using namespace eval;

/* public */

StdLib::StdLib(Log& log, Scope& rootScope)
  : m_log(log), m_scope(rootScope) {

  // object: the root of the object tree
  m_scope.newObject("object", auto_ptr<Type>(new NullType(m_log)));
  Object* object = new Object(m_log, "object", auto_ptr<Type>(new NullType(m_log)));
  m_scope.initObject("object", auto_ptr<Object>(object));

  m_scope.newObject("@", auto_ptr<Type>(new BasicType(m_log, *object)));
  m_scope.initObject("@",
      auto_ptr<Object>(new Object(m_log, "@",
          auto_ptr<Type>(new BasicType(m_log, *object)))));

  m_scope.commitAll();
}

// Nothing to do.  m_objects objects are owned by m_scope.
StdLib::~StdLib() {
}
