// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _TypeSpec_h_
#define _TypeSpec_h_

/* Type specifier
 *
 * This is like Expression except instead of wrapping any general expression,
 * it wraps an expression that we intend to use only for its Type.  At setup()
 * time we reorder and validate the operator tree just like an Expression.
 * Then we extract the Type of the expression and delete all the Nodes to block
 * them from ever being compiled.
 *
 * A TypeSpec will be more readily permitted to use | and maybe & than some
 * uses of Expression.  Additionally, a TypeSpec may be forbidden from
 * including object literals, by the parser.
 */

#include "OperatorParser.h"
#include "RootNode.h"
#include "Token.h"
#include "TypedNode.h"

#include "util/Log.h"

#include <string>

namespace compiler {

class TypeSpec : public TypedNode, public OperatorParser {
public:
  TypeSpec(Log& log, RootNode*const root, const Token& token)
    : TypedNode(log, root, token),
      OperatorParser(log) {}
  virtual void setup();

  std::string getTypeName() const;

private:
  // from TypedNode
  virtual void computeType();
};

}

#endif // _TypeSpec_h_
