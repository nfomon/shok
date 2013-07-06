// Copyright (C) 2013 Michael Biggs.  See the COPYRIGHT file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _Variable_h_
#define _Variable_h_

/* Variable */

#include "Log.h"
#include "Node.h"
#include "Object.h"
#include "RootNode.h"
#include "Token.h"
//#include "Type.h"

#include <string>

namespace eval {

class Variable : public Node {
public:
  Variable(Log& log, RootNode*const root, const Token& token)
    : Node(log, root, token) {}
  virtual void setup();
  virtual void analyzeUp();
  virtual void evaluate();

  //std::string varname() { return value; }

private:
  Object* m_object;
  //Type m_type;
};

};

#endif // _Variable_h_
