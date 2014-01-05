// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Arg_h_
#define _Arg_h_

/* Arg
 *
 * A Node representing a function argument as specified in the function's
 * declaration.  For example:
 *     new x = @(a:b, c) {}
 * There are two Args:  one is named a and has type b.  c is declared as a type
 * but with no name.
 */

#include "ArgSpec.h"
#include "Log.h"
#include "RootNode.h"
#include "Token.h"
#include "TypeSpec.h"

#include <memory>
#include <string>

namespace eval {

class Arg : public Node {
public:
  Arg(Log& log, RootNode*const root, const Token& token)
    : Node(log, root, token),
      m_typeSpec(NULL) {}
  virtual void setup();
  virtual void evaluate();

  // If the arg is unnamed, returns the empty string
  std::string getName() const;
  std::auto_ptr<Type> getType() const;
  std::auto_ptr<ArgSpec> getSpec() const;

private:
  std::string m_argname;  // provided by first child, if two children
  TypeSpec* m_typeSpec;   // last child
};

}

#endif // _Arg_h_
