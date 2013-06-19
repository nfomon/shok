#include "Block.h"

#include "EvalError.h"
#include "ExpressionBlock.h"

#include <string>
using std::string;

using namespace eval;

Block::Block(Log& log, const Token& token)
  : Brace(log, token, true),
    isCodeBlock(true) {
    //expression(NULL) {
}

Block::~Block() {
}

void Block::complete() {
  Brace::complete();

  // Determine if we're a code block or an expression block
  if (1 == children.size()) {
    ExpressionBlock* expBlock = dynamic_cast<ExpressionBlock*>(children.front());
    if (expBlock) {
      isCodeBlock = false;
    }
  }
}

void Block::evaluate() {
  if (isCodeBlock) {
    throw EvalError("Code block is unimplemented");
  } else if (children.size() != 1) {
    throw EvalError("Cannot evaluate block with deficient expblock");
  } else {
    children.front()->evaluate();
  }
}

string Block::cmdText() const {
  if (!isCodeBlock) {
    return children.front()->cmdText();
  }
  throw EvalError("CmdCodeBlock does not have a cmdText");
}
