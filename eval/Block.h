// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _Block_h_
#define _Block_h_

/* Block construct
 *
 * The Token (construction-time) does not have enough information to know if
 * this is a code block (list of statements) or an expression block (single
 * expression).  We have to wait until setup()-time to determine which.
 */

#include "Brace.h"
#include "ExpressionBlock.h"
#include "Log.h"
#include "RootNode.h"
#include "Token.h"
//#include "Statement.h"
#include "Variable.h"

#include <map>

namespace eval {

class Block : public Brace {
public:
  Block(Log& log, RootNode*const root, const Token& token)
    : Brace(log, root, token, true),
      m_scope(log),
      m_expBlock(NULL) {}
  ~Block();

  virtual void setup();
  virtual void analyzeDown();
  virtual void evaluate();
  virtual std::string cmdText() const;

  bool isCodeBlock() const { return !m_expBlock; }
  virtual Scope* getScope() { return &m_scope; }

private:
  ExpressionBlock* m_expBlock;
  Scope m_scope;
};

};

#endif // _Block_h_
