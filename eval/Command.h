// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Command_h_
#define _Command_h_

/* Command-line program invocation
 *
 * This is a Brace because of the way it happens to be represented in the
 * string AST we receive from the parser.
 */

#include "Brace.h"
#include "RootNode.h"
#include "Token.h"

#include "util/Log.h"

namespace compiler {

class Command : public Brace {
public:
  Command(Log& log, RootNode*const root, const Token& token)
    : Brace(log, root, token, true) {}
  virtual void setup();
  virtual void compile();
};

}

#endif // _Command_h_
