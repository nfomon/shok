#ifndef _RootNode_h_
#define _RootNode_h_

/* Root node of the AST */

#include "Log.h"
#include "Node.h"

#include <set>

namespace eval {

class RootNode : public Node {
public:
  RootNode(Log&);

  virtual void complete();
  // Reorder operator/expression trees for correct operator precedence
  virtual void reorderOperators();
  // Static analysis checks such as type checking and method lookups
  // Do as much as possible here to check for errors, short of causing
  // any external side-effects
  virtual void staticAnalysis();
  virtual void evaluate();

private:
  std::set<Node*> m_reordered;
  std::set<Node*> m_analyzed;
};

};

#endif // _RootNode_h_
