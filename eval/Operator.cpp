// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "Operator.h"

#include "EvalError.h"
#include "Function.h"

#include <boost/lexical_cast.hpp>

#include <memory>
#include <string>
using std::string;

using namespace eval;

/* public */

// Only do the most minimal of validation here.  We can't trust our children,
// but it is important that we know how many we have; that won't change.
void Operator::setup() {
  if (children.size() != 0) {
    throw EvalError("Operator must have no children at setup time; instead " + name + " has " + boost::lexical_cast<string>(children.size()));
  }
}

// Called by Expression::setup() top-down across its Operator tree.
void Operator::analyzeTree() {
  analysisSetup();
  log.debug("Before reordering: " + print());
  reorderOperatorTree();
  // After reordering, this may not be the top of the Operator tree any longer.
  // So find the new top, then validate operators top-down from there.
  Operator* top = this;
  while(dynamic_cast<Operator*>(top->parent)) {
    top = dynamic_cast<Operator*>(top->parent);
  }
  log.debug("After reordering: " + top->print());
  top->validateOperatorTree();
}

void Operator::evaluate() {
  // TODO remove this check once we're confident it can never happen
  if (!isReordered || !isValidated) {
    throw EvalError("Cannot evaluate operator '" + name + "' which has not been reordered and validated");
  }
  throw EvalError("Operator '" + name + "' not yet supported for evaluation");
}

string Operator::methodName() const {
  if ("EQ" == name)
    return "operator==";
  if ("NE" == name)
    return "operator!=";
  if ("LT" == name)
    return "operator<";
  if ("LE" == name)
    return "operator<=";
  if ("GT" == name)
    return "operator>";
  if ("GE" == name)
    return "operator>=";
  if ("USEROP" == name)
    return "operator`" + value + "`";
  if ("PLUS" == name)
    return "operator+";
  if ("MINUS" == name)
    return "operator-";
  if ("STAR" == name)
    return "operator*";
  if ("SLASH" == name)
    return "operator/";
  if ("PERCENT" == name)
    return "operator%";
  if ("CARAT" == name)
    return "operator^";
  return "";
}


/* protected */

void Operator::validate() {
  if (!isSetup || !isReordered) {
    throw EvalError("Cannot validate operator " + print() + " before it is setup and reordered");
  }

  if (isUnary && isBinary) {
    throw EvalError("Operator " + print() + " is both unary and binary. What?");
  } else if (isUnary) {
    if (children.size() != 1) {
      throw EvalError("Operator " + print() + " claims to be unary but has " + boost::lexical_cast<string>(children.size()) + " != 1 children");
    }
    left = dynamic_cast<TypedNode*>(children.at(0));
    if (!left) {
      throw EvalError("Operator " + print() + "'s child must have a Type");
    } else if (right) {
      throw EvalError("Cannot validate unary Operator " + print() + " that somehow has a right child");
    }
  } else if (isBinary) {
    if (children.size() != 2) {
      throw EvalError("Operator " + print() + " claims to be binary but has " + boost::lexical_cast<string>(children.size()) + " != 2 children");
    }
    left = dynamic_cast<TypedNode*>(children.at(0));
    right = dynamic_cast<TypedNode*>(children.at(1));
    if (!left || !right) {
      throw EvalError("Operator " + print() + "'s children must have Types");
    }
  } else {
    throw EvalError("Operator " + print() + " is neither unary nor binary..!?");
  }
  computeType();
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
  if ("paren" == name)
    return 15;
  throw EvalError("Unknown priority for operator '" + name + "'");
}

/* private */
void Operator::analysisSetup() {
  switch (children.size()) {
    case 1: isUnary = true; break;
    case 2: isBinary = true; break;
    default: throw EvalError("Operator " + print() + " must have one or two children");
  }
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    Operator* op = dynamic_cast<Operator*>(*i);
    if (op) {
      op->analysisSetup();
    }
  }
}

// Note: we know that this node and its children have all been setup, including
// analysisSetup().
void Operator::reorderOperatorTree() {
  log.debug("reordering " + print() + " with parent " + parent->print());
  if (isReordered) return;
  if (isBinary) {
    Operator* leftOp = dynamic_cast<Operator*>(children.at(0));
    Operator* rightOp = dynamic_cast<Operator*>(children.at(1));
    if (leftOp) {
      log.debug("parent " + print() + " reordering left " + leftOp->print());
      leftOp->reorderOperatorTree();
    }
    if (rightOp) {
      if (!parent || !rightOp->parent) {
        throw EvalError("Someone's parent is deficient");
      }
      if (rightOp->priority() > priority()) {
        log.debug("parent " + print() + " reordering right " + rightOp->print());
        rightOp->reorderOperatorTree();
      }
      // Careful: our right child might have changed!
      rightOp = dynamic_cast<Operator*>(children.at(1));
      if (rightOp->priority() <= priority()) {
        log.debug("parent " + print() + " shuffling");
        children.pop_back();
        children.push_back(rightOp->children.at(0));
        rightOp->parent = parent;
        parent = rightOp;
        rightOp->children.pop_front();
        rightOp->children.push_front(this);
        rightOp->parent->replaceChild(this, rightOp);
        log.debug("left child is now " + print() + ", now reordering its new parent " + rightOp->print() + " in shuffle");
        rightOp->reorderOperatorTree();
        log.debug("parent " + print() + " is shuffled");
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
  isValidated = true;
}

// This is responsible for setting m_type.  This is ok to be an OrType of the
// possible return types of the overloads of the method that will be called.
void Operator::computeType() {
  if (!isSetup || !isReordered) {
    throw EvalError("Cannot compute type of Operator " + print() + " before it is setup, reordered, and validated");
  }

  // For overloadable operators, see if the operand has implemented a method
  // for this operator.
  // If it's not overloadable, what are we doing here?  We probably should have
  // been subclassed :)  I think this is just | and & and ~, and for now we'll
  // implement all operator logic right here.
  // Note that some operators require specific types of their operands, or
  // other special evaluations (e.g. ~ performs a ->str on its operands).
  if ("PIPE" == name) {
    if (!isBinary) {
      throw EvalError("| must be a binary operator");
    }
    m_type.reset(new OrType(*left->getType(), *right->getType()));
  } else if ("AMP" == name) {
    if (!isBinary) {
      throw EvalError("& must be a binary operator");
    }
    m_type.reset(new AndType(*left->getType(), *right->getType()));
  } else if ("TILDE" == name || "DOUBLETILDE" == name) {
    if (!isBinary) {
      throw EvalError("| must be a binary operator");
    }
    // TODO: get this directly from the global scope
    const Object* str = parentScope->getObject("str");
    if (!str) {
      throw EvalError("Cannot use ~ or ~~ operator until str is defined");
    }
    m_type.reset(new BasicType(*str));
    // TODO more custom ~ and ~~ logic goes here or above
    throw EvalError("~ and ~~ operators are not yet implemented");
  } else {
    // Lookup the operator as a member of our first (or only) child's type.  If
    // it's there (and in the binary case, if it accepts the second child's
    // Type for its single argument), then our type is the (set of possible)
    // result type(s) of that method call.
    string method_name = methodName();
    if ("" == method_name) {
      throw EvalError("Cannot determine Type of unimplemented operator " + print());
    }
    const Object* methodObject = left->getType()->getMember(method_name);
    if (!methodObject) {
      throw EvalError(left->print() + " does not define operator" + name);
    }
    const Function* method = dynamic_cast<const Function*>(methodObject);
    if (!method) {
      throw EvalError("Somehow, " + method_name + " is a non-function member of " + left->print());
    }

    if (isUnary) {
      type_list args; // Leave empty (no args)
      if (!method->takesArgs(args)) {
        throw EvalError(left->print() + "." + method_name + " is not defined to take 0 arguments");
      }
      m_type = method->getPossibleReturnTypes(args);
      if (!m_type.get()) {
        throw EvalError(left->print() + "." + method_name + " somehow has no return type");
      }
    } else if (isBinary) {
      if (!right) {
        throw EvalError("Right-hand side of binary " + name + " operator must have a type");
      }
      type_list args;
      args.push_back(&right->type());
      if (!method->takesArgs(args)) {
        throw EvalError(left->print() + "." + method_name + " is not defined to take right-hand side " + right->print() + " of type " + args.at(0)->print());
      }
      m_type = method->getPossibleReturnTypes(args);
      if (!m_type.get()) {
        throw EvalError(left->print() + "." + method_name + " with argument " + right->print() + " somehow has no return type");
      }
    } else {
      throw EvalError("What IS this crazy operator!?");
    }
  }
}
