#ifndef _New_h_
#define _New_h_

/* New statement */

#include "Log.h"
#include "Node.h"
#include "RootNode.h"
#include "Token.h"

#include <string>

namespace eval {

class New : public Node {
public:
  New(Log& log, RootNode*const root, const Token& token)
    : Node(log, root, token) {}
  virtual void setup();
  virtual void evaluate();

private:
};

};

#endif // _New_h_
