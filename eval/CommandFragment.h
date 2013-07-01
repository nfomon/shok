#ifndef _CommandFragment_h_
#define _CommandFragment_h_

/* CommandFragment: a piece of a command-line */

#include "Log.h"
#include "Node.h"
#include "Token.h"

#include <string>

namespace eval {

class CommandFragment : public Node {
public:
  CommandFragment(Log& log, const Token& token)
    : Node(log, token) {}
  virtual void complete();
  virtual void evaluate();
  virtual std::string cmdText() const;
};

};

#endif // _CommandFragment_h_
