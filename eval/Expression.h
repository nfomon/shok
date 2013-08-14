// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _Expression_h_
#define _Expression_h_

/* Expression
 *
 * This just wraps an expression, which could be an atom (literal, variable
 * name, etc.) or the top operator of an expression tree.
 *
 * At setup() we reorder the operators (since they will be initially given to
 * the AST in a ridiculous ordering from the parser), then validate them
 * bottom-up.
 *
 * This single expression may be owned by a command block, in which case the
 * expression is meant to evaluate to an object on which we'll call
 * ->str.escape() and then provide this text as a command-line fragment.
 *
 * The expression's Type is determined at setup() time.
 */

#include "Log.h"
#include "RootNode.h"
#include "Token.h"
#include "Type.h"
#include "TypedNode.h"

#include <string>

namespace eval {

class Expression : public TypedNode {
public:
  Expression(Log& log, RootNode*const root, const Token& token)
    : TypedNode(log, root, token) {}
  virtual void setup();
  virtual void evaluate();
  virtual std::string cmdText() const;

private:
  // from TypedNode
  virtual void computeType();
};

};

#endif // _Expression_h_
