// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Expression.h"

#include "Operator.h"

#include "util/Util.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>
using std::auto_ptr;
using std::string;
using std::vector;

using namespace compiler;

Expression::Expression()
  : m_scope(NULL) {
}

void Expression::init(const Scope& scope) {
  m_scope = &scope;
}

void Expression::attach_atom(const Atom& atom) {
  m_stack.push_back(new AtomOperatorNode(atom));
}

void Expression::attach_preop(const std::string& preop) {
  m_stack.push_back(new PrefixOperatorNode(preop));
}

void Expression::attach_binop(const std::string& binop) {
  if (m_stack.empty()) {
    throw CompileError("Cannot attach infix operator " + binop + " at the start of an expression");
  }
  auto_ptr<InfixOperatorNode> op(new InfixOperatorNode(binop));
  stack_vec::auto_type tmp_node = m_stack.pop_back();
  if (!tmp_node || !dynamic_cast<AtomOperatorNode*>(tmp_node.get())) {
    throw CompileError("Cannot attach infix operator " + binop + " that does not follow an atom");
  }
  while (!m_stack.empty()) {
    if (op->priority() > m_stack.back().priority()) break;
    stack_vec::auto_type top_op = m_stack.pop_back();
    PrefixOperatorNode* top_preop = dynamic_cast<PrefixOperatorNode*>(top_op.get());
    InfixOperatorNode* top_infop = dynamic_cast<InfixOperatorNode*>(top_op.get());
    if (top_preop) {
      top_preop->addChild(tmp_node.release());
    } else if (top_infop) {
      top_infop->addRight(tmp_node.release());
    } else {
      throw CompileError("Found non-operator stack item at the not-top of the stack while attaching infix operator " + binop);
    }
    tmp_node.reset(top_op.release());
  }

  op->addLeft(tmp_node.release());
  m_stack.push_back(op);
}

void Expression::finalize() {
  if (m_stack.empty()) {
    throw CompileError("Cannot finalize Expression with empty stack");
  }
  stack_vec::auto_type tmp_node = m_stack.pop_back();
  if (!tmp_node || !dynamic_cast<AtomOperatorNode*>(tmp_node.get())) {
    throw CompileError("Cannot finalize Expression with dangling operator");
  }
  // The top of the stack is the missing child of the operator above it, and so
  // on up the stack.
  while (!m_stack.empty()) {
    stack_vec::auto_type top_op = m_stack.pop_back();
    PrefixOperatorNode* top_preop = dynamic_cast<PrefixOperatorNode*>(top_op.get());
    InfixOperatorNode* top_infop = dynamic_cast<InfixOperatorNode*>(top_op.get());
    if (top_preop) {
      top_preop->addChild(tmp_node.release());
    } else if (top_infop) {
      top_infop->addRight(tmp_node.release());
    } else {
      throw CompileError("Found non-operator stack item at the not-top of the stack while finalizing expression");
    }
    tmp_node.reset(top_op.release());
  }
  m_type = tmp_node->type();
  m_bytecode = tmp_node->bytecode();
}

std::string Expression::bytecode() const {
  return m_bytecode;
}

const Type& Expression::type() const {
  if (!m_type.get()) {
    throw CompileError("Cannot get type of untyped Expression");
  }
  return *m_type;
}
