#include "RootNode.h"

#include "Log.h"
#include "EvalError.h"
#include "Token.h"

using namespace eval;

RootNode::RootNode(Log& log)
  : Node(log, Token(":ROOT:")) {
}

void RootNode::complete() {
  throw EvalError("Cannot complete the root node");
}

void RootNode::reorderOperators() {
  //log.warning("Root node: operator reordering unimplemented");
}

void RootNode::staticAnalysis() {
  //log.warning("Root node: static analysis unimplemented");
}

void RootNode::evaluate() {
  log.debug("Evaluating root node");
  if (children.size() < 1) return;

  // Error if anyone other than the last child is incomplete
  for (child_iter i = children.begin(); i != children.end()-1; ++i) {
    if (!(*i)->isComplete() && i != children.end()-1) {
      throw EvalError("RootNode: found multiple incomplete children");
    }
  }
  int done = 0;
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    if ((*i)->isComplete()) {
      log.debug("Root node -- evaluating child " + (*i)->print());
      (*i)->evaluate();
      ++done;
    }
  }
  while (done > 0) {
    delete children.front();
    children.pop_front();
    --done;
  }
}
