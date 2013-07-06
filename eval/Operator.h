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
      isUnary(false),
      isBinary(false) {}
  virtual void setup();
  virtual void analyze();
  virtual void evaluate();

  virtual int priority() const;

private:
  bool isUnary;
  bool isBinary;
};

};

#endif // _Operator_h_
