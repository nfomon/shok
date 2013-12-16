// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "Operator.h"

#include "EvalError.h"
#include "Function.h"

#include <boost/lexical_cast.hpp>

#include <memory>
#include <string>
#include <utility>
using std::make_pair;
using std::string;

using namespace eval;

/* statics */

Operator::op_precedence Operator::Precedence(ARITY arity) {
  if (UNKNOWN_ARITY == arity) {
    throw EvalError("Cannot check unknown-arity precedence of " + print());
  }
  op_precedence prec;
  prec.priority = NO_PRIORITY;
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
  if ((INFIX == arity) && ("PLUS" == name || "MINUS" == name))
    prec.priority = 8;
  if ("STAR" == name || "SLASH" == name || "PERCENT" == name)
    prec.priority = 9;
  if ("CARAT" == name)
    prec.priority = 10;
    prec.assoc = RIGHT_ASSOC;
  if ("NOT" == name)
    prec.priority = 11;
  if ((PREFIX == arity) && ("PLUS" == name || "MINUS" == name))
    prec.priority = 12;
  if ("PIPE" == name)
    prec.priority = 13;
  if ("AMP" == name)
    prec.priority = 14;
  if ("paren" == name)
    prec.priority = 15;
  if (NO_PRIORITY == prec.priority) {
    throw EvalError("Failed to set Operator priority for " + print());
  }
  return prec;
}

/* public */

// OperatorParser will call this after each child has been setup with
// setupLeft() and/or setupRight().
void Operator::setup() {
  switch (m_arity) {
    case PREFIX:
      if (children.size() != 1) {
        throw EvalError("Prefix Operator " + print() + " must have one child");
      }
      break;
    case INFIX:
      if (children.size() != 2) {
        throw EvalError("Infix Operator " + print() + " must have 2 children");
      }
      break;
    default: throw EvalError("Cannot setup " + print() " + with unknown arity");
  }
}

bool Operator::couldBePrefix() const {
  if ("PLUS" == name || "MINUS" == name) return true;
  return false;
}

bool Operator::couldBeInfix() const {
  return true;
}

void Operator::setPrefix() {
  m_arity = PREFIX;
}

void Operator::setInfix() {
  m_arity = INFIX;
}

bool Operator::isPrefix() const {
  if (ARITY_UNKNOWN == m_arity) {
    throw EvalError("Cannot request isPrefix of " + print() + " with unassigned arity");
  }
  return PREFIX == m_arity;
}

bool Operator::isInfix() const {
  if (ARITY_UNKNOWN == m_arity) {
    throw EvalError("Cannot request isInfix of " + print() + " with unassigned arity");
  }
  return INFIX == m_arity;
}

void Operator::setupLeft() {
  if (ARITY_UNKNOWN == m_arity) {
    throw EvalError("Cannot call setupLeft() on unknown-arity " + print());
  }
  // ...
  if (INFIX != m_arity) {
    computeType();
  }
}

void Operator::setupRight() {
  if (ARITY_UNKNOWN == m_arity) {
    throw EvalError("Cannot call setupRight() on unknown-arity " + print());
  } else if (m_arity != INFIX) {
    throw EvalError("Cannot call setupRight() on non-infix " + print());
  }
  // ...
  computeType();
}

void Operator::evaluate() {
  // TODO remove this check once we're confident it can never happen
  if (!isSetup || (ARITY_UNKNOWN == m_arity)) {
    throw EvalError("Cannot evaluate operator '" + name + "' which has not been setup");
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


/* private */

// This is responsible for setting m_type.  This is ok to be an OrType of the
// possible return types of the overloads of the method that will be called.
void Operator::computeType() {
  if (!isSetup || (ARITY_UNKNOWN == m_arity)) {
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
