// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "Operator.h"

#include "EvalError.h"
#include "Function.h"

#include <boost/lexical_cast.hpp>

#include <memory>
#include <string>
#include <utility>
using std::string;
using std::make_pair;

using namespace eval;

/* statics */

bool Operator::CouldBeUnary(const std::string& name) {
  if ("PLUS" == name || "MINUS" == name) return true;
  return false;
}

bool Operator::CouldBeBinary(const std::string& name) {
  return true;
}

/* public */

// Only do the most minimal of validation here.  We can't trust our children,
// but it is important that we know how many we have; that won't change.
void Operator::setup() {
  if (children.size() != 0) {
    throw EvalError("Operator must have no children at setup time; instead " + name + " has " + boost::lexical_cast<string>(children.size()));
  }
}

void Operator::setUnary() {
  isUnary = true;
  isBinary = false;
  isOrderSet = true;
}

void Operator::setBinary() {
  isUnary = false;
  isBinary = true;
  isOrderSet = true;
}

// Called by Expression::setup() only at the top of the Operator tree.
void Operator::analyzeTree() {
  validateOperatorTree();
}

void Operator::evaluate() {
  // TODO remove this check once we're confident it can never happen
  if (!isSetup || !isOrderSet || !isValidated) {
    throw EvalError("Cannot evaluate operator '" + name + "' which has not been setup and validated");
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
  if (!isSetup || !isOrderSet) {
    throw EvalError("Cannot validate operator " + print() + " before it is setup");
  } else if (m_left || m_right) {
    throw EvalError("Operator " + print() + " has children before validation");
  }

  if (isUnary && isBinary) {
    throw EvalError("Operator " + print() + " is both unary and binary. What?");
  } else if (isUnary) {
    if (children.size() != 1) {
      throw EvalError("Operator " + print() + " claims to be unary but has " + boost::lexical_cast<string>(children.size()) + " != 1 children");
    }
    m_left = dynamic_cast<TypedNode*>(children.at(0));
    if (!m_left) {
      throw EvalError("Operator " + print() + "'s child must have a Type");
    }
  } else if (isBinary) {
    if (children.size() != 2) {
      throw EvalError("Operator " + print() + " claims to be binary but has " + boost::lexical_cast<string>(children.size()) + " != 2 children");
    }
    m_left = dynamic_cast<TypedNode*>(children.at(0));
    m_right = dynamic_cast<TypedNode*>(children.at(1));
    if (!m_left || !m_right) {
      throw EvalError("Operator " + print() + "'s children must have Types");
    }
  } else {
    throw EvalError("Operator " + print() + " is neither unary nor binary..!?");
  }
  computeType();
}

Operator::op_prec Operator::precedence() const {
  if (!isSetup || !isOrderSet) {
    throw EvalError("Cannot query priority of not-setup operator '" + name + "'");
  }
  op_prec prec;
  prec.priority = -1;
  prec.assoc = LEFT_ASSOC;
  //if ("COMMA_AND" == name)
  //  return 0;
  if ("DOT" == name)
    prec.priority = 1;
  if ("OR" == name || "NOR" == name || "XOR" == name || "XNOR" == name)
    prec.priority = 2;
  if ("AND" == name)
    prec.priority = 3;
  if ("EQ" == name || "NE" == name)
    prec.priority = 4;
  if ("LT" == name || "LE" == name || "GT" == name || "GE" == name)
    prec.priority = 5;
  if ("USEROP" == name)
    prec.priority = 6;
  if ("TILDE" == name || "DOUBLETILDE" == name)
    prec.priority = 7;
  if (isBinary && ("PLUS" == name || "MINUS" == name))
    prec.priority = 8;
  if ("STAR" == name || "SLASH" == name || "PERCENT" == name)
    prec.priority = 9;
  if ("CARAT" == name)
    prec.priority = 10;
    prec.assoc = RIGHT_ASSOC;
  if ("NOT" == name)
    prec.priority = 11;
  if (isUnary && ("PLUS" == name || "MINUS" == name))
    prec.priority = 12;
  if ("PIPE" == name)
    prec.priority = 13;
  if ("AMP" == name)
    prec.priority = 14;
  if ("paren" == name)
    prec.priority = 15;
  if (-1 == prec.priority) {
    throw EvalError("Failed to set priority for Operator " + print());
  }
  return prec;
}

/* private */

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
  if (!isSetup || !isOrderSet || !isValidated) {
    throw EvalError("Cannot compute type of Operator " + print() + " before it is setup and validated");
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
    m_type.reset(new OrType(*m_left->getType(), *m_right->getType()));
  } else if ("AMP" == name) {
    if (!isBinary) {
      throw EvalError("& must be a binary operator");
    }
    m_type.reset(new AndType(*m_left->getType(), *m_right->getType()));
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
    const Object* methodObject = m_left->getType()->getMember(method_name);
    if (!methodObject) {
      throw EvalError(m_left->print() + " does not define operator" + name);
    }
    const Function* method = dynamic_cast<const Function*>(methodObject);
    if (!method) {
      throw EvalError("Somehow, " + method_name + " is a non-function member of " + m_left->print());
    }

    if (isUnary) {
      type_list args; // Leave empty (no args)
      if (!method->takesArgs(args)) {
        throw EvalError(m_left->print() + "." + method_name + " is not defined to take 0 arguments");
      }
      m_type = method->getPossibleReturnTypes(args);
      if (!m_type.get()) {
        throw EvalError(m_left->print() + "." + method_name + " somehow has no return type");
      }
    } else if (isBinary) {
      if (!m_right) {
        throw EvalError("Right-hand side of binary " + name + " operator must have a type");
      }
      type_list args;
      args.push_back(&m_right->type());
      if (!method->takesArgs(args)) {
        throw EvalError(m_left->print() + "." + method_name + " is not defined to take right-hand side " + m_right->print() + " of type " + args.at(0)->print());
      }
      m_type = method->getPossibleReturnTypes(args);
      if (!m_type.get()) {
        throw EvalError(m_left->print() + "." + method_name + " with argument " + m_right->print() + " somehow has no return type");
      }
    } else {
      throw EvalError("What IS this crazy operator!?");
    }
  }
}
