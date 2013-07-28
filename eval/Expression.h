// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _Expression_h_
#define _Expression_h_

/* Expression
 *
 * This just wraps an expression, which could be an atom (literal, variable
 * name, etc.) or the top operator of an expression tree.
 *
 * At setup() we reorder the operators (since they will be provided to us in a
 * ridiculous ordering from the parser), then validate them bottom-up.
 *
 * This single expression may be owned by a command block, in which case the
 * expression is meant to evaluate to an object on which we'll call
 * ->str().escape() and then provided this text as a command-line fragment.
 * Currently we assume the ->str() does not throw and thus requires no
 * validation.  However, TODO we should know (via init()) if we're an
 * expression from an expression block, and validate in setup() that this
 * ->str() is at least reasonable.
 */

#include "Log.h"
#include "Node.h"
#include "RootNode.h"
#include "Token.h"

#include <string>

namespace eval {

class Expression : public Node {
public:
  Expression(Log& log, RootNode*const root, const Token& token)
    : Node(log, root, token) {}
  virtual void setup();
  virtual void evaluate();
  virtual std::string cmdText() const;

private:
  std::string m_str;
};

};

#endif // _Expression_h_
