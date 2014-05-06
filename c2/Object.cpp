// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Object.h"

#include "util/Util.h"

#include <boost/lexical_cast.hpp>
using boost::lexical_cast;

#include <string>
#include <utility>
using std::string;

using namespace compiler;

Object::Object()
  : m_scope(NULL) {
}

void Object::init(Scope& scope) {
  m_scope = &scope;
}

const Type& Object::type() const {
  if (!m_type) {
    throw CompileError("Cannot get type of untyped Object");
  }
  return *m_type;
}
