#ifndef _RootNode_h_
#define _RootNode_h_

/* Root node of the AST */

#include "Log.h"
#include "Node.h"

namespace eval {

struct RootNode : public Node {
  RootNode(Log& log);

  virtual void complete();
  virtual void evaluate();
};

};

#endif // _RootNode_h_
