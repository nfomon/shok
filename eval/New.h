// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _New_h_
#define _New_h_

/* New statement */

#include "Log.h"
#include "RootNode.h"
#include "Statement.h"
#include "Token.h"

#include <string>

namespace eval {

class New : public Statement {
public:
  New(Log& log, RootNode*const root, const Token& token)
    : Statement(log, root, token) {}

  virtual void setup();
  virtual void analyze();
  virtual void evaluate();

private:
};

};

#endif // _New_h_
