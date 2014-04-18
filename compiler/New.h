// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _New_h_
#define _New_h_

/* New statement */

#include "RootNode.h"
#include "Statement.h"
#include "Token.h"

#include "util/Log.h"

#include <string>

namespace compiler {

class New : public Statement {
public:
  New(Log& log, RootNode*const root, const Token& token)
    : Statement(log, root, token) {}

  virtual void setup();
  virtual void analyze();
  virtual void compile();

private:
};

}

#endif // _New_h_
