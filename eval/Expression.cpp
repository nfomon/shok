// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "Expression.h"

#include "EvalError.h"
#include "Operator.h"
#include "Variable.h"

#include <boost/lexical_cast.hpp>

#include <string>
#include <vector>
using std::string;
using std::vector;

using namespace eval;

void Expression::setup() {
  if (children.size() != 1) {
    throw EvalError("Expression " + print() + " must have exactly one child");
  }
  computeType();
}

// Nothing to do here
void Expression::evaluate() {
}

string Expression::cmdText() const {
  if (!isEvaluated) {
    throw EvalError("Cannot get cmdText of unevaluated Expression");
  }
  // TODO: call the resulting object's ->str.escape() (*UNIMPL*)
  //return children.front()->cmdText();
  return "Expression__cmdText unimplemented";
}

Object& Expression::getObject() const {
  if (!isEvaluated) {
    throw EvalError("Cannot get object from Expression " + print() + " before its evaluation");
  }
  Variable* var = dynamic_cast<Variable*>(children.at(0));
  if (var) {
    return var->getObject();
  }
  // TODO: Operator
  // TODO: Object
  // TODO: Function
  throw EvalError("Expression " + print() + " cannot retrieve Object from unsupported child type " + children.at(0)->print());
}

Node* Expression::makeOperatorTree(child_vec& nodes, Operator::op_priority p) {
  if (0 == nodes.size()) {
    return NULL;
  }
  // prefix
  Node* top = nodes.front();
  nodes.pop_front();
  Variable* var = dynamic_cast<Variable*>(top);
  Operator* op = dynamic_cast<Operator*>(top);
  if (var) {
    log.debug(" = prefix var " + var->print());
  } else if (op && Operator::CouldBeUnary(op->name)) {
    log.debug(" = prefix op " + op->print());
    op->setUnary();
    Node* operand = makeOperatorTree(nodes, op->precedence().priority);
    if (!operand) { throw EvalError("a"); }
    log.debug(" = prefix operand " + operand->print());
    operand->parent = op;
    op->addChild(operand);
    op->setUnary();
  } else { throw EvalError("b"); }
  if (0 == nodes.size()) {
    log.debug(" unary outta nodes!");
    return top;
  }
  // lookahead
  Node* second = nodes.front();
  Operator* op2 = dynamic_cast<Operator*>(second);
  // infix
  while (op2 && Operator::CouldBeBinary(op2->name)) {
    log.debug(" = infix op " + op2->print());
    op2->setBinary();
    Operator::op_prec infix_prec = op2->precedence();
    if (p >= infix_prec.priority) {
      log.debug(" = -> above priority; skipping");
      break;
    }
    log.debug(" = -> below priority");
    nodes.pop_front();
    op2->addChild(top);
    Node* operand = makeOperatorTree(nodes, infix_prec.priority - (int)infix_prec.assoc);
    log.debug(" = infix op " + op2->print()  + " operand " + operand->print());
    if (!operand) { throw EvalError("c"); }
    op2->addChild(operand);
    top = op2;
    if (0 == nodes.size()) {
      log.debug(" infix op " + op2->print() + " outta nodes!");
      break;
    }
    second = nodes.front();
    op2 = dynamic_cast<Operator*>(second);
  }
  return top;
}

void Expression::computeType() {
  TypedNode* child = dynamic_cast<TypedNode*>(children.at(0));
  if (!child) {
    throw EvalError("Child of Expression must be a TypedNode");
  }
  m_type = child->getType();
  if (!m_type.get()) {
    throw EvalError("Child of Expression must have a Type");
  }
}
