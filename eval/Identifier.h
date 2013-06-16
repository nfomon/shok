#ifndef _Identifier_h_
#define _Identifier_h_

/* Identifier
 *
 * This could be a variable name, or on the command-line, just some (partial)
 * text of a program name or argument.
 */

#include "Log.h"
#include "Node.h"
#include "Token.h"

#include <string>

namespace eval {

class Identifier : public Node {
public:
  Identifier(Log& log, const Token& token)
    : Node(log, token) {}
  virtual void complete();
  virtual void evaluate();
  virtual std::string cmdText() const;

private:
};

};

#endif // _Identifier_h_
