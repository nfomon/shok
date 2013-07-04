#include "Block.h"

#include "EvalError.h"
#include "ExpressionBlock.h"

#include <string>
using std::string;

using namespace eval;

Block::Block(Log& log, const Token& token)
  : Brace(log, token, true),
    expBlock(NULL) {
    //expression(NULL) {
}

Block::~Block() {
}

void Block::complete() {
  Brace::complete();
  m_isComplete = false;

  if (expBlock) {
    throw EvalError("Block.expBlock should not already be set when completing");
  }
  // Determine if we're a code block or an expression block
  if (1 == children.size()) {
    expBlock = dynamic_cast<ExpressionBlock*>(children.front());
  }
  m_isComplete = true;
}

void Block::evaluate() {
  if (!expBlock) {
    throw EvalError("Code block is unimplemented");
  } else {
    expBlock->evaluate();
  }
}

string Block::cmdText() const {
  if (!expBlock) {
    throw EvalError("Cannot get cmdText of a code block");
  }
  return expBlock->cmdText();
}
