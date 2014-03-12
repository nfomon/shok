// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _TypedNode_h_
#define _TypedNode_h_

/* TypedNode
 *
 * Any AST Node that has a Type and thus may appear in an Expression.
 */

#include "EvalError.h"
#include "Node.h"
#include "RootNode.h"
#include "Token.h"
#include "Type.h"

#include "util/Log.h"

#include <memory>

namespace eval {

class TypedNode : public Node {
public:
  TypedNode(Log& log, RootNode*const root, const Token& token)
    : Node(log, root, token),
      m_type(NULL) {}
  virtual ~TypedNode() {}

  // Returns a duplicate of m_type; caller must take ownership
  std::auto_ptr<Type> getType() const;

  // Use this for quick const lookups on the Type without duplicating it
  const Type& type() const;

protected:
  virtual void computeType() = 0;
  std::auto_ptr<Type> m_type;
};

}

#endif // _TypedNode_h_
