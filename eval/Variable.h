// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _Variable_h_
#define _Variable_h_

/* Variable
 *
 * A Variable simply refers to an object by name.  In most uses, the Object
 * already exists, and at setup() we determine our Type (a BasicType that wraps
 * the existing Object).  If the Object does not exist, then we might be the
 * left-hand side of a New statement which is going to create one.  In this
 * case we leave m_object and m_type both NULL, and the NewInit will use our
 * variable name (stored as our 'value' member) to create the new Object.
 */

#include "Log.h"
#include "Node.h"
#include "Object.h"
#include "RootNode.h"
#include "Token.h"
#include "Type.h"
#include "TypedNode.h"

#include <string>

namespace eval {

class Variable : public TypedNode {
public:
  Variable(Log& log, RootNode*const root, const Token& token)
    : TypedNode(log, root, token),
      m_object(NULL) {}
  virtual void setup();
  virtual void evaluate();

  std::string getVariableName() const { return value; }

private:
  // from TypedNode
  virtual void computeType();
  const Object* m_object;
};

};

#endif // _Variable_h_
