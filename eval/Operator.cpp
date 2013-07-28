// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "Operator.h"

#include "EvalError.h"

#include <string>
using std::string;

using namespace eval;

/* public */

void Operator::setup() {
  switch (children.size()) {
    case 1: isUnary = true; break;
    case 2: isBinary = true; break;
    default: throw EvalError("Operator must have one or two children");
  }
}

void Operator::analyzeTree() {
  reorderOperatorTree();
  validateOperatorTree();
}

void Operator::evaluate() {
  // TODO remove this check once we're confident it can never happen
  if (!isReordered || !isValidated) {
    throw EvalError("Cannot evaluate operator '" + name + "' which has not been reordered and validated");
  }
  throw EvalError("Operator '" + name + "' not yet supported for evaluation");
}

/* protected */

void Operator::validate() {
  if (!isReordered) {
    throw EvalError("Cannot validate operator " + print() + " which has not been reordered");
  }
  // For overloadable operators, see if the operand has implemented a method
  // for this operator.
  // If it's not overloadable, what are we doing here?  We probably should have
  // been subclassed :)
  // Note that some operators require specific types of their operands, or
  // other special evaluations (e.g. ~ performs a ->str() on its operands).
  //if (isOverloadable) {
    // Unary => Prefix operator, for now
  //} else {
  //}

  // Our type is the type of the result of the operation
  isValidated = true;
}

int Operator::priority() const {
  if (!isSetup) {
    throw EvalError("Cannot query priority of not-setup operator '" + name + "'");
  }
  //if ("COMMA_AND" == name)
  //  return 0;
  if ("DOT" == name)
    return 1;
  if ("OR" == name || "NOR" == name || "XOR" == name || "XNOR" == name)
    return 2;
  if ("AND" == name)
    return 3;
  if ("EQ" == name || "NE" == name)
    return 4;
  if ("LT" == name || "LE" == name || "GT" == name || "GE" == name)
    return 5;
  if ("USEROP" == name)
    return 6;
  if ("TILDE" == name || "DOUBLETILDE" == name)
    return 7;
  if (isBinary && ("PLUS" == name || "MINUS" == name))
    return 8;
  if ("STAR" == name || "SLASH" == name || "PERCENT" == name)
    return 9;
  if ("CARAT" == name)
    return 10;
  if ("NOT" == name)
    return 11;
  if (isUnary && ("PLUS" == name || "MINUS" == name))
    return 12;
  if ("PIPE" == name)
    return 13;
  if ("AMP" == name)
    return 14;
  if ("paren" == name || "bracket" == name)
    return 15;
  throw EvalError("Unknown priority for operator '" + name + "'");
}

/* private */

// Note: we know that this node and its children have all been setup.
void Operator::reorderOperatorTree() {
  log.debug("reordering " + print());
  if (isReordered) return;
  if (2 == children.size()) {
    Operator* leftOp = dynamic_cast<Operator*>(children.at(0));
    Operator* rightOp = dynamic_cast<Operator*>(children.at(1));
    if (leftOp) {
      leftOp->reorderOperatorTree();
    }
    if (rightOp) {
      if (!parent || !rightOp->parent) {
        throw EvalError("Someone's parent is deficient");
      }
      if (rightOp->priority() > priority()) {
        rightOp->reorderOperatorTree();
      } else {
        children.pop_back();
        children.push_back(rightOp->children.at(0));
        rightOp->parent = parent;
        parent = rightOp;
        rightOp->children.pop_front();
        rightOp->children.push_front(this);
        rightOp->parent->replaceChild(this, rightOp);
        rightOp->reorderOperatorTree();
      }
    }
  }
  log.debug(" - reordered node " + print());
  isReordered = true;
}

// Child-first validation
void Operator::validateOperatorTree() {
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    Operator* op = dynamic_cast<Operator*>(*i);
    if (op) {
      op->validateOperatorTree();
    }
  }
  validate();
}
