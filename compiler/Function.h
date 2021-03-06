// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Function_h_
#define _Function_h_

/* Function
 *
 * Function represents a single function signature possibly with a body.  (If
 * there's no body, then it's an abstract.)  Function has/owns/creates an
 * Object which will have a single function member for this signature.  When a
 * Function is &'d with another Function or Object, these function-members will
 * either merge in the ObjectStore, or conflict if there's a duplicate
 * builtin-function-taking-compatible-args.
 *
 * Two signatures on the same function, for now, must have different #
 * arguments.  This will eventually be relaxed to: lining up the arguments, no
 * OR-item from one can be identical to an OR-item of the other.   e.g.
 * @(A|B)&X  <=>  @(C|A)&X  (not allowed) Different overloads may have the same
 * or different return types.
 *
 * The function does not take ownership of any Type* or Object* it is given as
 * part of an Arg.  These must and will outlive the Function.
 *
 * Note that a function that "returns void" does not leave its returntype NULL,
 * it actually should have a void object that presently does not exist...
 */

#include "Args.h"
#include "Block.h"
#include "Returns.h"
#include "Token.h"
#include "TypedNode.h"

#include "util/Log.h"

#include <memory>
#include <string>

namespace compiler {

class Block;

class Function : public TypedNode {
public:
  Function(Log& log, RootNode*const root, const Token& token)
    : TypedNode(log, root, token),
      m_args(NULL),
      m_returns(NULL),
      m_body(NULL),
      m_preparedArgs(false),
      m_preparedReturns(false),
      m_isObjectified(false) {}

  virtual void initChild(Node* child);
  virtual void setup();
  virtual void compile();

  arg_vec getArgs() const;

  // Make an Object out of this Function.  Transfers ownership of m_body.
  // This can only happen once, because ownership of m_body is transferred.
  // TODO: also transfer ownership of m_args and m_returns, instead of copying
  // them.
  std::auto_ptr<Object> makeObject(const std::string& newName);

private:
  // from TypedNode
  virtual void computeType();

  Args* m_args;
  Returns* m_returns;
  Block* m_body;

  bool m_preparedArgs;
  bool m_preparedReturns;
  bool m_isObjectified;   // set by makeObject()
};

}

#endif // _Function_h_
