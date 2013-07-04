#ifndef _Operator_h_
#define _Operator_h_

/* Operator */

#include "Log.h"
#include "Node.h"
#include "Token.h"

#include <string>

namespace eval {

class Operator : public Node {
public:
  Operator(Log& log, const Token& token)
    : Node(log, token),
      isCmd(false),
      isUnary(false),
      isBinary(false) {}
  virtual void complete();
  virtual void evaluate();

  virtual int priority() const;

private:
  bool isCmd;
  bool isUnary;
  bool isBinary;
};

};

#endif // _Operator_h_
