// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "RootNode.h"

#include "Log.h"
#include "EvalError.h"
#include "Token.h"

#include <memory>
using std::auto_ptr;

using namespace eval;

/* public */

RootNode::RootNode(Log& log)
  : Node(log, NULL, Token(":ROOT:")),
    m_scope(log) {
  isInit = true;
  isSetup = true;
  isAnalyzed = true;

  // Insert default objects (standard library)
  Object& object = m_scope.newObject("object", auto_ptr<Type>(new NullType()));
  //object.newMember("foo", std::auto_ptr<Type>(new BasicType(object))); // test
  Object& func = m_scope.newObject("@", auto_ptr<Type>(new BasicType(object)));
  // Test: member function
  Object& foo = object.newMember("foo", std::auto_ptr<Type>(new BasicType(func)));
  //std::auto_ptr<Function> foofunc(new Function(log, 
  m_scope.commitAll();
}

// Reset any pending-evaluation children, and undo any pending changes
// to the scope.  Note that this does not clear out variables that are
// already commit in the global scope; we only let that happen on
// destruction.
void RootNode::reset() {
  log.debug("Resetting root node");
  clearChildren(false);
  m_scope.revertAll();
}

void RootNode::prepare() {
  isEvaluated = false;
}

/* protected */

void RootNode::setup() {
  throw EvalError("Cannot setup the root node");
}

void RootNode::evaluate() {
  // Children were evaluated successfully.  Clear them away.
  clearChildren(true);
}

void RootNode::clearChildren(bool onlyEvaluatedChildren) {
  int n = children.size();
  if (onlyEvaluatedChildren) {
    log.debug("Clearing root node's evaluated children");
    n = 0;
    for (child_iter i = children.begin(); i != children.end(); ++i) {
      if (!(*i)->isNodeEvaluated()) {
        break;
      }
      ++n;
    }
  } else {
    log.debug("Clearing root node's children");
  }
  while (n > 0) {
    delete children.front();
    children.pop_front();
    --n;
  }
}
