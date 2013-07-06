// Copyright (C) 2013 Michael Biggs.  See the COPYRIGHT file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _RootNode_h_
#define _RootNode_h_

/* Root node of the AST */

#include "Global.h"
#include "Log.h"
#include "Node.h"

#include <set>

namespace eval {

class RootNode : public Node {
public:
  RootNode(Log&);

  // Reset the whole AST; destroys all children
  void reset();
  // Prepare the RootNode for another evaluation
  void prepare();

  virtual void setup();
  virtual void evaluate();

  Global global;

private:
  void clearChildren(bool onlyEvaluatedChildren = false);
};

};

#endif // _RootNode_h_
