#ifndef _Comma_h_
#define _Comma_h_

/* Comma operator
 *
 * Used as part of the string representation of an AST passed in by the parser,
 * it may have special meaning.  On the command-line, it represents a "hard
 * space", i.e. an actual separator between a program name and its arguments.
 */

#include "Log.h"
#include "Node.h"
#include "Token.h"

#include <string>

namespace eval {

class Comma : public Node {
public:
  Comma(Log& log, const Token& token)
    : Node(log, token) {}
  virtual void complete();
  virtual void evaluate();
  virtual std::string cmdText() const;

private:
};

};

#endif // _Comma_h_
