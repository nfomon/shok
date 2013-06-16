#include "Block.h"

#include <string>
using std::string;

using namespace eval;

Block::Block(Log& log, const Token& token)
  : Brace(log, token, true),
    isCodeBlock(false) {
    //expression(NULL) {
}

Block::~Block() {
}

void Block::complete() {
  Brace::complete();

  // Determine if we're a code block or an expression block
  //if (1 == children.size()) {
  //  expression = dynamic_cast<Expression*>(children.front());
  //  if (expression) {
  //    isCodeBlock = false;
  //  }
  //}
}

void Block::evaluate() {
}

string Block::cmdText() const {
  return "";
}
