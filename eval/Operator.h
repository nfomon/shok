// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _Operator_h_
#define _Operator_h_

/* Operator */

#include "Log.h"
#include "Node.h"
#include "RootNode.h"
#include "Token.h"

#include <string>

namespace eval {

class Operator : public Node {
public:
  Operator(Log& log, RootNode*const root, const Token& token)
    : Node(log, root, token),
      isUnary(false),
      isBinary(false) {}
  virtual void setup();
  virtual void analyzeUp();
  virtual void evaluate();

  virtual int priority() const;

private:
  bool isUnary;
  bool isBinary;
};

};

#endif // _Operator_h_
