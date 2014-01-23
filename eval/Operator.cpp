// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Operator.h"

#include "EvalError.h"
#include "Function.h"

#include <boost/lexical_cast.hpp>

#include <memory>
#include <string>
#include <utility>
using std::auto_ptr;
using std::make_pair;
using std::string;

using namespace eval;

/* public */

// OperatorParser will call this after all children have been setup with
// setupLeft() and/or setupRight().
void Operator::setup() {
  if (!isPartiallySetup || !m_type.get()) {
    throw EvalError("Cannot setup Operator " + print() + " that has not been partially-setup");
  } else if (isSetup) {
    throw EvalError("Cannot setup Operator " + print() + " that has already been setup");
  }
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
    default: throw EvalError("Cannot setup " + print() + " with unknown arity");
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

Operator::op_precedence Operator::precedence(ARITY arity) {
  if (ARITY_UNKNOWN == arity) {
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

void Operator::setupLeft() {
  if (ARITY_UNKNOWN == m_arity) {
    throw EvalError("Cannot call setupLeft() on unknown-arity " + print());
  } else if (isPartiallySetup) {
    throw EvalError("Cannot call setupLeft() on " + print() + " that is already partially-setup");
  } else if (children.size() != 1) {
    throw EvalError("Cannot setupLeft() on " + print() + " that does not have exactly one child");
  }
  m_left = dynamic_cast<TypedNode*>(children.at(0));
  if (!m_left) {
    throw EvalError("Cannot setupLeft() on " + print() + " whose child is not a TypedNode");
  }
  // ...
  if (INFIX != m_arity) {
    isPartiallySetup = true;
    computeType();
  }
}

void Operator::setupRight() {
  if (ARITY_UNKNOWN == m_arity) {
    throw EvalError("Cannot call setupRight() on unknown-arity " + print());
  } else if (m_arity != INFIX) {
    throw EvalError("Cannot call setupRight() on non-infix " + print());
  } else if (isPartiallySetup) {
    throw EvalError("Cannot call setupRight() on " + print() + " that is already partially-setup");
  } else if (children.size() != 2) {
    throw EvalError("Cannot setupRight() on " + print() + " that does not have two children");
  }
  m_left = dynamic_cast<TypedNode*>(children.at(0));
  m_right = dynamic_cast<TypedNode*>(children.at(1));
  if (!m_left || !m_right) {
    throw EvalError("Cannot setupRight() on " + print() + " whose children are not TypedNodes");
  }
  // ...
  isPartiallySetup = true;
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
// This is called before setup(), by setupLeft() for prefix and setupRight()
// for infix operators.
void Operator::computeType() {
  if (!isPartiallySetup || (ARITY_UNKNOWN == m_arity)) {
    throw EvalError("Cannot compute type of Operator " + print() + " before it is setup and validated");
  } else if (m_type.get()) {
    throw EvalError("Cannot compute type of Operator " + print() + " that already has a type");
  }

  // For overloadable operators, see if the operand has implemented a method
  // for this operator.
  // If it's not overloadable, what are we doing here?  We probably should have
  // been subclassed :)  I think this is just | and & and ~, and for now we'll
  // implement all operator logic right here.
  // Note that some operators require specific types of their operands, or
  // other special evaluations (e.g. ~ performs a ->str on its operands).
  if ("PIPE" == name) {
    if (!isInfix()) {
      throw EvalError("| must be a binary operator");
    }
    m_type.reset(new OrType(log, *m_left->getType(), *m_right->getType()));
  } else if ("AMP" == name) {
    if (!isInfix()) {
      throw EvalError("& must be a binary operator");
    }
    m_type.reset(new AndType(log, *m_left->getType(), *m_right->getType()));
  } else if ("TILDE" == name || "DOUBLETILDE" == name) {
    if (!isInfix()) {
      throw EvalError("| must be a binary operator");
    }
    const Symbol* str = root->getScope()->getSymbol("str");
    if (!str) {
      throw EvalError("Cannot use ~ or ~~ operator until str is defined");
    }
    m_type.reset(new BasicType(log, *str));
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
    /*
    auto_ptr<Type> method = m_left->type().getMemberType(method_name);
    if (!method.get()) {
      throw EvalError(m_left->print() + " does not define operator" + name);
    }
    */

    if (isPrefix()) {
      paramtype_vec params;   // Leave empty (no params)
      // TODO when we re-implement takesArgs() etc.
      /*
      if (!method->takesArgs(params)) {
        throw EvalError(m_left->print() + "." + method_name + " is not defined to take 0 arguments");
      }
      m_type = method->getPossibleReturnTypes(params);
      if (!m_type.get()) {
        throw EvalError(m_left->print() + "." + method_name + " somehow has no return type");
      }
    } else if (isInfix()) {
      if (!m_right) {
        throw EvalError("Right-hand side of binary " + name + " operator must have a type");
      }
      paramtype_vec params;
      params.push_back(&m_right->type());
      if (!method->takesArgs(params)) {
        throw EvalError(m_left->print() + "." + method_name + " is not defined to take right-hand side " + m_right->print() + " of type " + params.at(0)->print());
      }
      m_type = method->getPossibleReturnTypes(params);
      if (!m_type.get()) {
        throw EvalError(m_left->print() + "." + method_name + " with argument " + m_right->print() + " somehow has no return type");
      }
      */
    } else {
      throw EvalError("What IS this crazy operator!?");
    }
  }
}
