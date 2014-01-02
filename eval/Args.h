// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Args_h_
#define _Args_h_

/* Args
 *
 * A list of Arg, as seen in a function definition.
 */

#include "Arg.h"
#include "Log.h"
#include "RootNode.h"
#include "Token.h"

#include <vector>

namespace eval {

class Args : public Node {
public:
  Args(Log& log, RootNode*const root, const Token& token)
    : Node(log, root, token) {}
  virtual void setup();
  virtual void evaluate();

  typedef std::vector<Arg*> args_vec;
  typedef args_vec::const_iterator args_iter;
  args_vec getArgs() const;    // would rather provide iterators. meh.

private:
  args_vec m_args;
};

};

#endif // _Args_h_
