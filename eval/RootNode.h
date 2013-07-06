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

  // Reset flags to prepare for re-evaluation
  void reset();

  virtual void complete();
  virtual void evaluate();

private:
};

};

#endif // _RootNode_h_
