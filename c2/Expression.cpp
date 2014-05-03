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

void Expression::init(Scope& scope) {
  m_scope = &scope;
}

void Expression::attach_atom(const Variable& atom) {
/*
  // For now the Expression has a single Variable atom.  Our type is that the
  // Expression is a descendent of the variable, and adds no new members.
  m_type.reset(new BasicType(atom.type().duplicate(), atom.fullname()));
  m_bytecode = atom.fullname();
*/
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
  stack_vec::auto_type tmp_op = m_stack.pop_back();
  if (!tmp_op || !dynamic_cast<AtomOperatorNode*>(tmp_op.get())) {
    throw CompileError("Cannot attach infix operator " + binop + " that does not follow an atom");
  }
  while (!m_stack.empty()) {
    if (op->priority() > m_stack.back().priority()) break;
    stack_vec::auto_type top_op = m_stack.pop_back();
    PrefixOperatorNode* top_preop = dynamic_cast<PrefixOperatorNode*>(top_op.get());
    InfixOperatorNode* top_infop = dynamic_cast<InfixOperatorNode*>(top_op.get());
    if (top_preop) {
      top_preop->addChild(tmp_op.release());
    } else if (top_infop) {
      top_infop->addRight(tmp_op.release());
    } else {
      throw CompileError("Found non-operator stack item at the not-top of the stack while attaching infix operator " + binop);
    }
    tmp_op.reset(top_op.release());
  }

  op->addLeft(tmp_op.release());
  m_stack.push_back(op);
}

void Expression::finalize() {
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
