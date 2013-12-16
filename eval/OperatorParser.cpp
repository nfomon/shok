// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "OperatorParser.h"

#include "EvalError.h"
#include "Operator.h"
#include "Variable.h"

#include <string>
#include <utility>
#include <vector>
using std::make_pair;
using std::string;
using std::vector;

using namespace eval;

/* public */

// Pratt (TDOP: Top-Down Operator Precedence) parser, accepting nodes
// one-at-a-time and manipulating an explicit stack.
void OperatorParser::insertNode(Node* node) {
  Operator* op = dynamic_cast<Operator*>(node);

  Node* stackTop = NULL;
  Operator::op_priority topPriority = Operator::NO_PRIORITY;
  if (!m_stack.empty()) {
    pair<Node*,Operator::op_priority> top = m_stack.back();   // peek, don't pop
    stackTop = top.first;
    if (!stackTop) {
      throw EvalError("Found deficient stack top while parsing " + print());
    }
    topPriority = top.second;
  }
  Operator* stackOp = dynamic_cast<Operator*>(stackTop);

  // If we're looking for an infix operator, and that's what we've found
  if (m_infixing && op && op->couldBeInfix()) {
    Operator::op_precedence opPrecedence = op->Precedence(Operator::INFIX);
    op->setInfix();

    // Top of stack is a free non-operator.  Everything above it is an operator.
    if (stackOp || !stackTop || topPriority != Operator::NO_PRIORITY) {
      throw EvalError(print() + " beginning infix parse, yet top of stack is an operator");
    }
    Node* tmp = stackTop;
    m_stack.pop();
    // Fight over tmp against any higher-priority operators up the stack
    while (!m_stack.empty()) {
      pair<Node*,Operator::op_priority> top = m_stack.back(); // peek, don't pop
      stackTop = top.first;
      if (!stackTop) {
        throw EvalError("Found deficient stack top while parsing " + print());
      }
      topPriority = top.second;
      stackOp = dynamic_cast<Operator*>(stackTop);
      if (!stackOp || Operator::NO_PRIORITY == topPriority) {
`       throw EvalError("ab");
      }
      if (opPrecedence.priority > topPriority) break;
      // stackOp consumes tmp, then becomes tmp, and we bubble up the stack.
      stackOp->addChild(tmp);
      tmp->parent = stackOp;
      if (stackOp->isPrefix()) {
        stackOp->setupLeft();
      } else if (stackOp->isInfix()) {
        stackOp->setupRight();
      } else {
        throw EvalError("bad arity");
      }
      stackOp->setup();
      tmp = stackOp;
      m_stack.pop();
    }

    // op consumes tmp
    op->addChild(tmp);
    tmp->parent = op;
    op->setupLeft();

    // Finally, push our op onto the stack and return.
    m_stack.push_back(op, opPrecedence.priority + (int)opPrecedence.assoc);
    m_infixing = false;
    return;
  }

  if (op && op->couldBePrefix()) {
    op->setPrefix();
    m_stack.push_back(make_pair(node, op->Precedence(Operator::PREFIX)));
    // Prefix operator short-circuits the infixing: leave m_infixing==false
    return;
  }
  // Non-operator symbol: set m_infixing=true to check for right-side ops
  m_stack.push_back(make_pair(node, Operator::NO_PRIORITY));
  m_infixing = true;
}

Node* OperatorParser::finalizeParse() {
  if (m_stack.empty()) {
    throw EvalError("Finalizing parse of empty OperatorParser " + print());
  }
  // Ensure we were not left with a dangling operator
  Node* stackTop = m_stack.back();
  Operator* stackOp = dynamic_cast<Operator*>(stackTop);
  if (stackOp) {
    throw EvalError("Finalizing parse of " + print() + " was left with dangling operator " + stackOp->print());
  }
  // The top of the stack fills a void in the operator above it, and so on up
  // the stack.
  Node* tmp = stackTop;
  while (!m_stack.empty()) {
    
  }
  return NULL;
}
