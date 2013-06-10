#ifndef _Code_h_
#define _Code_h_

/* Code execution */

#include "Log.h"
#include "Node.h"

#include <string>

namespace eval {

class Node;

class Code {
public:
  typedef std::string Result;

  static Result Operator(Log&, Node*);
  static Result Block(Log&, Node*);
  static Result Cmd(Log&, Node*);
  static Result New(Log&, Node*);
};

};

#endif // _Code_h_
