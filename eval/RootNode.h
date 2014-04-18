// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _RootNode_h_
#define _RootNode_h_

/* Root node of the AST
 *
 * Its children are all Commands, although this is not enforced.
 */

#include "Node.h"
#include "Scope.h"
#include "StdLib.h"

#include "util/Log.h"

namespace compiler {

class RootNode : public Node {
public:
  RootNode(Log&);

  // Reset the whole AST; destroys all children
  void reset();
  // Prepare the RootNode for another compilation
  void prepare();

  virtual Scope* getScope() { return &m_scope; }

protected:
  virtual void setup();
  virtual void compile();

private:
  void clearChildren();
  Scope m_scope;
  StdLib m_stdlib;
};

}

#endif // _RootNode_h_
