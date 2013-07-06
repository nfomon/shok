#include "RootNode.h"

#include "Log.h"
#include "EvalError.h"
#include "Token.h"

using namespace eval;

/* public */

RootNode::RootNode(Log& log)
  : Node(log, Token(":ROOT:")) {
    m_isSetup = true;
    m_isComplete = true;
}

void RootNode::reset() {
  m_isReordered = false;
  m_isAnalyzed = false;
  m_isEvaluated = false;
}

/* protected */

void RootNode::complete() {
  throw EvalError("Cannot complete the root node");
}

void RootNode::evaluate() {
  log.debug("Evaluating root node");
  int done = 0;
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    if (!(*i)->isEvaluated()) {
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
