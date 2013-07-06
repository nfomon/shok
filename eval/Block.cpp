#include "Block.h"

#include "EvalError.h"
#include "ExpressionBlock.h"

#include <string>
using std::string;

using namespace eval;

Block::Block(Log& log, const Token& token)
  : Brace(log, token, true),
    m_expBlock(NULL) {
    //expression(NULL) {
}

Block::~Block() {
}

void Block::setup() {
  Brace::setup();

  if (m_expBlock) {
    throw EvalError("Block.m_expBlock should not already be set when setting up");
  }
  // Determine if we're a code block or an expression block
  if (1 == children.size()) {
    m_expBlock = dynamic_cast<ExpressionBlock*>(children.front());
  }
}

void Block::evaluate() {
  if (!m_expBlock) {
    throw EvalError("Code block is unimplemented");
  } else {
    m_expBlock->evaluate();
  }
}

string Block::cmdText() const {
  if (!m_expBlock) {
    throw EvalError("Cannot get cmdText of a code block");
  }
  return m_expBlock->cmdText();
}

bool Block::isInScope(Variable* v) const {
  if (m_variables.end() == m_variables.find(v)) {
    if (block) {
      return block->isInScope(v);
    } else {
      return false;
    }
  }
  return true;
}

void Block::addVariable(Variable* v) {
  if (m_variables.find(v) != m_variables.end()) {
    throw EvalError("Block " + print() + " cannot add variable " + v->print() + "; name conflicts in local scope");
  }
  m_variables.insert(v);
}
