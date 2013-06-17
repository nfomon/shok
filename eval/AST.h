#ifndef _AST_h_
#define _AST_h_

/* Abstract Syntax Tree */

#include "EvalError.h"
#include "Log.h"
#include "Node.h"
#include "RootNode.h"
#include "Token.h"

#include <string>

namespace eval {

class AST {
public:
  AST(Log& log);
  ~AST();

  // Performs the ugly work of inserting an input "AST Token" into the AST.
  void insert(const Token& token);
  // Reset the AST to a correct state; may destroy some unevaluated code.
  void reset();
  // Analyze the AST and execute any appropriate, complete fragments of code
  void evaluate();
  // Pretty-print the contents of the AST to a string
  std::string print() const;

private:
  // Complete all of a node's children, then the node itself
  // This should be moved to Node, as a public non-virtual complete().  Node
  // then will have a protected _complete() that this will call.
  static void completeNode(Node*);

  void init();
  void destroy();

  // Reorder operator/expression trees for correct operator precedence
  void reorderOperators();
  void reorderOps(Node*) const;
  // Static analysis checks such as type checking and method lookups
  void staticAnalysis();
  // Actually run the code!
  void runCode();

  Log& m_log;
  RootNode* m_top;
  Node* m_current;
};

};

#endif // _AST_h_
