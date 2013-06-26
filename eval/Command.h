#ifndef _Command_h_
#define _Command_h_

/* Command-line program invocation
 *
 * This is a Brace because of the way it happens to be represented in the
 * string AST we receive from the parser.
 *
 * Note that our children include "comma" nodes that should cmdText() to a
 * space character.  Everything else needs to be evaluated into strings that we
 * can concatenate directly adjacent each other to form the user's
 * command-line.
 */

#include "Brace.h"
#include "Log.h"
#include "Token.h"

namespace eval {

class Command : public Brace {
public:
  Command(Log& log, const Token& token)
    : Brace(log, token, true) {}
  virtual void complete();
  virtual void evaluate();
};

};

#endif // _Command_h_
