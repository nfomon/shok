// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "OperatorParser.h"

#include "EvalError.h"
#include "Operator.h"
#include "Variable.h"

#include <string>
#include <utility>
using std::make_pair;
using std::string;

using namespace eval;

/* public */

// Pratt (TDOP: Top-Down Operator Precedence) parser, accepting nodes
// one-at-a-time and manipulating an explicit stack.
void OperatorParser::insertNode(Node* node) {
  m_log.info("Operator parser parsing node " + node->print());

  Operator* op = dynamic_cast<Operator*>(node);
  Node* stackTop = NULL;
  Operator::op_priority topPriority = Operator::NO_PRIORITY;
  if (!m_stack.empty()) {
    stack_pair top = m_stack.back();   // peek, don't pop
    stackTop = top.first;
    if (!stackTop) {
      throw EvalError("Found deficient stack top while parsing " + node->print());
    }
    topPriority = top.second;
  }
  Operator* stackOp = dynamic_cast<Operator*>(stackTop);

  // If we're looking for an infix operator, and that's what we've found
  if (m_infixing && op && op->couldBeInfix()) {
    Operator::op_precedence opPrecedence = op->precedence(Operator::INFIX);
    op->setInfix();

    // Top of stack is a free non-operator.  Everything above it is an operator.
    if (stackOp || !stackTop || topPriority != Operator::NO_PRIORITY) {
      throw EvalError(node->print() + " beginning infix parse, yet top of stack is an operator");
    }
    Node* tmp = stackTop;
    m_stack.pop_back();
    // Fight over tmp against any higher-priority operators up the stack
    while (!m_stack.empty()) {
      stack_pair top = m_stack.back(); // peek, don't pop
      stackTop = top.first;
      if (!stackTop) {
        throw EvalError("Found deficient stack top while parsing " + node->print());
      }
      topPriority = top.second;
      stackOp = dynamic_cast<Operator*>(stackTop);
      if (!stackOp || Operator::NO_PRIORITY == topPriority) {
        throw EvalError("Operator parse of " + node->print() + " found not-top non-op stack item, or deficient priority");
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
        throw EvalError("Operator parse of " + node->print() + " found unknown arity");
      }
      stackOp->setupNode();
      tmp = stackOp;
      m_stack.pop_back();
    }

    // op consumes tmp
    op->addChild(tmp);
    tmp->parent = op;
    op->setupLeft();

    // Finally, push our op onto the stack and return.
    m_stack.push_back(make_pair(op, opPrecedence.priority + (int)opPrecedence.assoc));
    m_infixing = false;
    return;
  }

  if (op && op->couldBePrefix()) {
    op->setPrefix();
    m_stack.push_back(make_pair(node, op->precedence(Operator::PREFIX).priority));
    // Prefix operator short-circuits the infixing: leave m_infixing==false
    return;
  }
  // Non-operator symbol: set m_infixing=true to check for right-side ops
  m_stack.push_back(make_pair(node, Operator::NO_PRIORITY));
  m_infixing = true;
}

Node* OperatorParser::finalizeParse() {
  m_log.info("Finalizing operator parser");

  if (m_stack.empty()) {
    throw EvalError("Finalizing parse of empty OperatorParser");
  }
  // Ensure we were not left with a dangling operator
  Node* stackTop = m_stack.back().first;
  Operator* stackOp = dynamic_cast<Operator*>(stackTop);
  if (stackOp) {
    throw EvalError("Finalizing parse was left with dangling operator " + stackOp->print());
  }
  // The top of the stack is the missing child of the operator above it, and so
  // on up the stack.
  Node* tmp = stackTop;
  m_stack.pop_back();
  stackTop->setupNode();

  while (!m_stack.empty()) {
    stack_pair top = m_stack.back(); // peek, don't pop
    stackTop = top.first;
    if (!stackTop) {
      throw EvalError("Finalizing parse found deficient stack top");
    }
    stackOp = dynamic_cast<Operator*>(stackTop);
    if (!stackOp || Operator::NO_PRIORITY == top.second) {
      throw EvalError("Finalizing parse found not-top non-op stack item, or deficient priority");
    }
    // stackOp consumes tmp, then becomes tmp, and we bubble up the stack.
    stackOp->addChild(tmp);
    tmp->parent = stackOp;
    if (stackOp->isPrefix()) {
      stackOp->setupLeft();
    } else if (stackOp->isInfix()) {
      stackOp->setupRight();
    } else {
      throw EvalError("Finalizing parse found unknown arity");
    }
    stackOp->setupNode();
    tmp = stackOp;
    m_stack.pop_back();
  }

  return tmp;
}
