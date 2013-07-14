// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _ExpressionBlock_h_
#define _ExpressionBlock_h_

/* ExpressionBlock
 *
 * This wraps an expression that should be evaluated and returned to the
 * command-line.  That is, we'll evaluate the expression, safely escape its
 * string representation, and return that as the cmdText() to be included as
 * part of a command invocation.
 */

#include "Log.h"
#include "Node.h"
#include "RootNode.h"
#include "Token.h"

#include <string>

namespace eval {

class ExpressionBlock : public Node {
public:
  ExpressionBlock(Log& log, RootNode*const root, const Token& token)
    : Node(log, root, token) {}
  virtual void setup();
  virtual void evaluate();
  virtual std::string cmdText() const;

private:
  std::string m_str;
};

};

#endif // _ExpressionBlock_h_
