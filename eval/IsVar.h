// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _IsVar_h_
#define _IsVar_h_

/* IsVar statement
 *
 * This is an ugly interpreter built-in being temporarily implemented as a
 * statement, that somehow magically displays a true/false value to the user
 * via hacks.
 */

#include "Log.h"
#include "RootNode.h"
#include "Token.h"

#include <string>

namespace eval {

class IsVar : public Node {
public:
  IsVar(Log& log, RootNode*const root, const Token& token)
    : Node(log, root, token) {}
  virtual void setup();
  virtual void evaluate();

private:
};

};

#endif // _IsVar_h_
