// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Identifier_h_
#define _Identifier_h_

/* Identifier
 *
 * A minimal label that refers to an object or object-member that may or may
 * not yet exist.  Held by nodes such as Variable and NewInit.
 */

#include "Log.h"
#include "Node.h"
#include "RootNode.h"
#include "Token.h"

#include <string>

namespace eval {

class Identifier : public Node {
public:
  Identifier(Log& log, RootNode*const root, const Token& token)
    : Node(log, root, token) {}
  virtual void setup();
  virtual void evaluate();

  std::string getName() const { return value; }

private:
};

};

#endif // _Identifier_h_
