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

#include <set>

namespace eval {

class Block : public Brace {
public:
  Block(Log& log, RootNode*const root, const Token& token)
    : Brace(log, root, token, true),
      m_expBlock(NULL) {}
  ~Block() {}

  virtual void setup();
  virtual void evaluate();
  virtual std::string cmdText() const;

  bool isCodeBlock() const { return !m_expBlock; }
  bool isInScope(Variable*) const;
  void addVariable(Variable*);

private:
  ExpressionBlock* m_expBlock;
  std::set<Variable*> m_variables;
  Global* m_global;
};

};

#endif // _Block_h_
