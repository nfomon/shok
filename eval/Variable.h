#ifndef _Variable_h_
#define _Variable_h_

/* Variable */

#include "Log.h"
#include "Node.h"
#include "Token.h"

#include <string>

namespace eval {

class Variable : public Node {
public:
  Variable(Log& log, const Token& token)
    : Node(log, token) {}
  virtual void complete();
  virtual void analyze();
  virtual void evaluate();

private:
};

};

#endif // _Variable_h_
