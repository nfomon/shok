// Copyright (C) 2013 Michael Biggs.  See the COPYRIGHT file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "RootNode.h"

#include "Log.h"
#include "EvalError.h"
#include "Token.h"

using namespace eval;

/* public */

RootNode::RootNode(Log& log)
  : Node(log, NULL, Token(":ROOT:")),
    m_scope(log) {
  isSetup = true;
}

void RootNode::reset() {
  clearChildren(false);
  m_scope.cleanup();
}

void RootNode::prepare() {
  isReordered = false;
  isAnalyzed = false;
  isEvaluated = false;
}

/* protected */

void RootNode::setup() {
  throw EvalError("Cannot setup the root node");
}

void RootNode::evaluate() {
  // Children were evaluated successfully.  Clear them away.
  clearChildren(true);
  m_scope.cleanup();
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
