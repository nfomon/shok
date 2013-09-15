// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _ProcCall_h_
#define _ProcCall_h_

/* ProcCall (the calling of a function/procedure/method) */

#include "Function.h"
#include "Log.h"
#include "Signature.h"
#include "Type.h"
#include "Variable.h"

#include <memory>
#include <string>

namespace eval {

class ProcCall : public TypedNode {
public:
  ProcCall(Log& log, RootNode*const root, const Token& token)
    : TypedNode(log, root, token) {}
  virtual void setup();
  virtual void evaluate();

private:
  // from TypedNode
  virtual void computeType();
  Function* m_function;
  std::vector<Expression*> m_argexps;
};

};

#endif // _ProcCall_h_
