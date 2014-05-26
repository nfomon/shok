// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Operator.h"

#include <string>
#include <utility>
#include <vector>
using std::string;
using std::vector;

using namespace compiler;

/* Operators */

Operator::Priority PrefixOperator::PrefixPriority(const std::string& name) {
  if ("PLUS" == name || "MINUS" == name) {
    return 12;
  }
  throw CompileError("Priority unknown for prefix operator " + name);
}

Operator::Priority InfixOperator::InfixPriority(const std::string& name) {
  if ("DOT" == name) {
    return 1;
  } else if ("OR" == name || "NOR" == name || "XOR" == name || "XNOR" == name) {
    return 2;
  } else if ("AND" == name) {
    return 3;
  } else if ("EQ" == name || "NE" == name) {
    return 4;
  } else if ("LT" == name || "LE" == name || "GT" == name || "GE" == name) {
    return 5;
  } else if ("USEROP" == name) {
    return 6;
  } else if ("TILDE" == name || "DOUBLETILDE" == name) {
    return 7;
  } else if ("PLUS" == name || "MINUS" == name) {
    return 8;
  } else if ("STAR" == name || "SLASH" == name || "PERCENT" == name) {
    return 9;
  } else if ("CARAT" == name) {
    return 10;
  } else if ("NOT" == name) {
    return 11;
  } else if ("PIPE" == name) {
    return 13;
  } else if ("AMP" == name) {
    return 14;
  } else if ("paren" == name) {
    return 15;
  }
  throw CompileError("Priority unknown for infix operator " + name);
}

InfixOperator::Assoc InfixOperator::assoc(const std::string& name) {
  if ("CARAT" == name) {
    return RIGHT_ASSOC;
  }
  return LEFT_ASSOC;
}

/* Operator tree nodes */

std::string OperatorNode::bytecode() const {
  if (m_bytecode.empty()) {
    throw CompileError("Cannot get bytecode from incompletely-compiled Operator tree");
  }
  return m_bytecode;
}

boost::shared_ptr<Type> OperatorNode::type() const {
  if (!m_type.get()) {
    throw CompileError("OperatorNode '" + m_bytecode + "' does not have a type");
  }
  return m_type;
}

void PrefixOperatorNode::addChild(OperatorNode* child) {
  if (m_child.get()) {
    throw CompileError("Cannot add another child to PrefixOperator " + m_op.name());
  }
  m_child.reset(child);
  // TODO check that the child implements this method (no args), and set m_type
  // to the type of the result
  m_bytecode = " (call " + m_child->bytecode() + " " + m_op.methodName() + ")";
}

void InfixOperatorNode::addLeft(OperatorNode* left) {
  if (m_left.get()) {
    throw CompileError("Cannot add another left child to InfixOperator " + m_op.name());
  }
  // TODO check that the left child implements this method (for one unknown arg)
  m_left.reset(left);
}

void InfixOperatorNode::addRight(OperatorNode* right) {
  if (m_right.get()) {
    throw CompileError("Cannot add another right child to InfixOperator " + m_op.name());
  }
  m_right.reset(right);
  // TODO check that the left child implements this method with the right child
  // as its argument, and set m_type to the type of the result
  m_bytecode = " (call " + m_left->bytecode() + " " + m_op.methodName() + " " + m_right->bytecode() + ")";
}
