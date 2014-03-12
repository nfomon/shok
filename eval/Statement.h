// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Statement_h_
#define _Statement_h_

/* Statement
 *
 * Abstract base class for statements.  A statement has an analyze() method,
 * called by Node::analyzeNode(), that performs whatever static analysis is
 * appropriate for that statement.
 */

#include "Node.h"
#include "RootNode.h"
#include "Token.h"

#include "util/Log.h"

namespace eval {

class Statement : public Node {
public:
  Statement(Log& log, RootNode*const root, const Token& token)
    : Node(log, root, token) {}

  virtual void analyze() {}
  virtual bool isInstant() const { return false; }
};

}

#endif // _Statement_h_
