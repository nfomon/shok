// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "TypedNode.h"

#include <memory>
using std::auto_ptr;

using namespace eval;

// Returns a duplicate of m_type; caller must take ownership
auto_ptr<Type> TypedNode::getType() const {
  if (!m_type.get()) {
    throw EvalError("Cannot get Type of TypedNode " + print() + " before it has been computed");
  }
  return m_type->duplicate();
}

const Type& TypedNode::type() const {
  if (!m_type.get()) {
    throw EvalError("Cannot refer to Type of TypedNode " + print() + " before it has been computed");
  }
  return *m_type.get();
}
