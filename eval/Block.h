// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Block_h_
#define _Block_h_

/* Block construct
 *
 * The Token (construction-time) does not have enough information to know if
 * this is a code block (list of statements) or an expression block (single
 * expression).  We have to wait until setup()-time to determine which.
 */

#include "Brace.h"
#include "Expression.h"
#include "Function.h"
#include "util/Log.h"
#include "NewInit.h"
#include "ObjectLiteral.h"
#include "RootNode.h"
#include "Scope.h"
#include "Statement.h"
#include "Token.h"

#include <map>

namespace eval {

class ObjectLiteral;

class Block : public Brace {
public:
  Block(Log& log, RootNode*const root, const Token& token)
    : Brace(log, root, token, true),
      m_scope(log),
      m_exp(NULL),
      m_function(NULL),
      m_object(NULL),
      m_isDeferred(false) {}
  ~Block();

  virtual void initScope(Scope* scopeParent);
  virtual void initScope(Scope* scopeParent, Function* function);
  virtual void initScope(Scope* scopeParent, ObjectLiteral* object);
  virtual void setup();
  virtual void evaluate();
  virtual std::string cmdText() const;

  bool isCodeBlock() const { return !m_exp; }
  virtual Scope* getScope() { return &m_scope; }
  void defer() { m_isDeferred = true; }
  void ready() { m_isDeferred = false; }
  bool isDeferred() const { return m_isDeferred; }
  std::vector<NewInit*> getInits() const;
  //std::auto_ptr<Block> duplicate() const;

  Scope::depth_t depth() const { return m_scope.depth(); }

private:
  typedef std::vector<Statement*> statement_vec;
  typedef statement_vec::const_iterator statement_iter;

  void codegen();

  Scope m_scope;
  Expression* m_exp;            // Set if this is an expression block
  statement_vec m_statements;   // Only used by code-block
  Function* m_function;         // Set if this is a function's block
  ObjectLiteral* m_object;      // Set if this is an object literal's block
  bool m_isDeferred;
};

}

#endif // _Block_h_
