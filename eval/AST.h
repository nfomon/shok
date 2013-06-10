#ifndef _AST_h_
#define _AST_h_

#include "EvalError.h"
#include "Log.h"
#include "Node.h"
#include "Token.h"

#include <string>

namespace eval {

class AST {
public:
  AST(Log& log);
  ~AST();

  void insert(const Token& token);
  void reset();
  void evaluate();
  std::string print() const;

private:
  void reorderOperators();
  void checkTypes();
  void runCode();

  Log& m_log;
  Node* m_top;
  Node* m_current;
};

};

#endif // _AST_h_
