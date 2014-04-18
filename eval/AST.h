// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _AST_h_
#define _AST_h_

/* Abstract Syntax Tree */

#include "Node.h"
#include "RootNode.h"
#include "Token.h"

#include "util/Log.h"

#include <string>

namespace compiler {

class AST {
public:
  AST(Log& log);
  ~AST();

  // Reset the AST to a correct state; may destroy some uncompiled code.
  void reset();
  // Performs the ugly work of inserting an input "AST Token" into the AST.
  void insert(const Token& token);
  // Analyze the AST and compile any appropriate, complete fragments of code
  void compile();
  // Pretty-print the contents of the AST to a string
  std::string print() const;

private:
  Log& m_log;
  RootNode m_root;
  Node* m_current;
};

}

#endif // _AST_h_
