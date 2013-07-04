#ifndef _New_h_
#define _New_h_

/* New statement */

#include "Log.h"
#include "Node.h"
#include "Token.h"

#include <string>

namespace eval {

class New : public Node {
public:
  New(Log& log, const Token& token)
    : Node(log, token) {}
  virtual void complete();
  virtual void evaluate();

private:
};

};

#endif // _New_h_
