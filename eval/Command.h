#ifndef _Command_h_
#define _Command_h_

/* Command-line program invocation
 *
 * Note that our children include "comma" nodes that should cmdText() to a
 * space character.  Everything else needs to be evaluated into strings that we
 * can concatenate directly adjacent each other to form the user's
 * command-line.
 */

#include "Node.h"

namespace eval {

class Command : public Node {
public:
  Command(Log& log, const Token& token)
    : Node(log, token) {}
  virtual void complete();
  virtual void evaluate();

private:
};

};

#endif // _Command_h_
