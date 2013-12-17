// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Expression_h_
#define _Expression_h_

/* Expression
 *
 * This wraps an expression.  Each child could be an atom (literal, variable
 * name, etc.) or the top operator of an expression tree.
 *
 * The operators are given to us by the AST (parser) in a ridiculous and
 * flattened ordering, so at setup() time we must arrange our children into a
 * tree and then reorder them for operator precedence.  Then we validate the
 * operators bottom-up.
 *
 * This single expression may be owned by an expression block, in which case
 * the expression is meant to evaluate to an object on which we'll call
 * ->str.escape() and then provide this text as a command-line fragment.
 *
 * The expression's Type is determined at setup() time.
 */

#include "Log.h"
#include "Operator.h"
#include "OperatorParser.h"
#include "RootNode.h"
#include "Token.h"
#include "Type.h"
#include "TypedNode.h"

#include <string>

namespace eval {

class Expression : public TypedNode, public OperatorParser {
public:
  Expression(Log& log, RootNode*const root, const Token& token)
    : TypedNode(log, root, token),
      OperatorParser(log) {}
  virtual void setup();
  virtual void evaluate();
  virtual std::string cmdText() const;

  // Get the resulting Object after this Expression has been evaluated.
  // Note that this Object& will not stick around for long!  It has not been
  // saved to a scope.
  Object& getObject() const;

private:
  // from TypedNode
  virtual void computeType();
};

};

#endif // _Expression_h_
