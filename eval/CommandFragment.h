// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _CommandFragment_h_
#define _CommandFragment_h_

/* CommandFragment: a piece of a command-line */

#include "Log.h"
#include "Node.h"
#include "RootNode.h"
#include "Token.h"

#include <string>

namespace eval {

class CommandFragment : public Node {
public:
  CommandFragment(Log& log, RootNode*const root, const Token& token)
    : Node(log, root, token) {}
  virtual void setup();
  virtual void evaluate();
  virtual std::string cmdText() const;
};

};

#endif // _CommandFragment_h_
