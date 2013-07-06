#include "RootNode.h"

#include "Log.h"
#include "EvalError.h"
#include "Token.h"

using namespace eval;

/* public */

RootNode::RootNode(Log& log)
  : Node(log, Token(":ROOT:")) {
    isSetup = true;
}

void RootNode::reset() {
  isReordered = false;
  isAnalyzed = false;
  isEvaluated = false;
}

/* protected */

void RootNode::setup() {
  throw EvalError("Cannot setup the root node");
}

void RootNode::evaluate() {
  log.debug("Evaluating root node");
  int done = 0;
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    if (!(*i)->isNodeEvaluated()) {
      break;
    }
    ++done;
  }
  while (done > 0) {
    delete children.front();
    children.pop_front();
    --done;
  }
}
