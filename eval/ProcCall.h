// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _ProcCall_h_
#define _ProcCall_h_

/* ProcCall (the calling of a function/procedure/method) */

#include "Common.h"
#include "Function.h"
#include "Log.h"
#include "Object.h"
#include "Type.h"
#include "Variable.h"

#include <memory>
#include <string>

namespace eval {

class ProcCall : public TypedNode {
public:
  ProcCall(Log& log, RootNode*const root, const Token& token)
    : TypedNode(log, root, token),
      m_object(NULL) {}
  virtual void setup();
  virtual void evaluate();

private:
  // from TypedNode
  virtual void computeType();
  Object* m_object;
  typedef std::vector<Expression*> exp_vec;
  typedef exp_vec::const_iterator exp_iter;
  exp_vec m_paramexps;
  paramtype_vec m_paramtypes;
};

}

#endif // _ProcCall_h_
