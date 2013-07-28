// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _RootNode_h_
#define _RootNode_h_

/* Root node of the AST */

#include "Log.h"
#include "Node.h"
#include "Scope.h"

#include <set>

namespace eval {

class RootNode : public Node {
public:
  RootNode(Log&);

  // Reset the whole AST; destroys all children
  void reset();
  // Prepare the RootNode for another evaluation
  void prepare();

protected:
  virtual void setup();
  virtual void evaluate();
  virtual Scope* getScope() { return &m_scope; }

private:
  void clearChildren(bool onlyEvaluatedChildren = false);
  Scope m_scope;
};

};

#endif // _RootNode_h_
