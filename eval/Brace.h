#ifndef _Brace_h_
#define _Brace_h_

/* Brace -- an AST token that requires a match */

#include "Log.h"
#include "Node.h"
#include "Token.h"

namespace eval {

class Brace : public Node {
public:
  Brace(Log&, const Token&, bool isOpen);
  virtual ~Brace();

  bool isOpen() const;
  bool isIrrelevant() const;
  bool matchesCloseBrace(Brace* closeBrace) const;

  virtual void complete();
  virtual void evaluate();
  virtual std::string cmdText() const;

private:
  bool m_isOpen;
};

};

#endif // _Brace_h_
