// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "RootNode.h"

#include "EvalError.h"
#include "Token.h"

#include "util/Log.h"

using namespace eval;

/* public */

RootNode::RootNode(Log& log)
  : Node(log, NULL, Token(":ROOT:")),
    m_scope(log),
    m_stdlib(log, m_scope) {
  isInit = true;
  isSetup = true;
  isAnalyzed = true;
}

// Reset any pending-evaluation children, and undo any pending changes
// to the scope.  Note that this does not clear out variables that are
// already commit in the global scope; we only let that happen on
// destruction.
void RootNode::reset() {
  log.debug("Resetting root node");
  clearChildren();
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
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    (*i)->evaluateNode();
    // TODO we could delete the child here, but that's messy to deal with
  }
  clearChildren();
}

void RootNode::clearChildren() {
  while (!children.empty()) {
    delete children.front();
    children.pop_front();
  }
}
