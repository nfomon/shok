#ifndef _AST_h_
#define _AST_h_

#include "Log.h"
#include "Node.h"
#include "Token.h"

#include <stdexcept>
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

class ASTError : public std::runtime_error {
public:
  ASTError(const std::string& what) : std::runtime_error(what) {}
};

};

#endif // _AST_h_
