// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Args_h_
#define _Args_h_

/* Args
 *
 * A list of Arg, as seen in a function definition.
 */

#include "Arg.h"
#include "Common.h"
#include "RootNode.h"
#include "Token.h"

#include "util/Log.h"

#include <vector>

namespace compiler {

class Args : public Node {
public:
  Args(Log& log, RootNode*const root, const Token& token)
    : Node(log, root, token) {}
  virtual void setup();

  const arg_vec& getArgs() const;

private:
  arg_vec m_args;
};

}

#endif // _Args_h_
