// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _NewInit_h_
#define _NewInit_h_

/* New statement's variable-initialization helper
 *
 * There are three forms of NewInit statement-fragments:
 *    new x           equivalent to   new x = object = object
 *    new x = y       equivalent to   new x = typeof(y) = y
 *    new x = y = z   equivalent to   new x = y = z
 */

#include "Expression.h"
#include "Log.h"
#include "Node.h"
#include "RootNode.h"
#include "Scope.h"
#include "Token.h"
#include "TypeSpec.h"
#include "Variable.h"

#include <string>

namespace eval {

class NewInit : public Node {
public:
  NewInit(Log& log, RootNode*const root, const Token& token)
    : Node(log, root, token),
      m_isPrepared(false),
      m_variable(NULL),
      m_exp(NULL),
      m_typeSpec(NULL),
      m_type(NULL),
      m_object(NULL) {}
  ~NewInit();

  virtual void setup();
  // Prepare our scope to include the new variable (analysis step).
  // This is called by an analysis step kickstarted by our parent New, after
  // its setup() is completed.
  // We could do this same work at setup() time but it seems nicer to let the
  // New statement finish its setup() validation beforehand.  Here we actually
  // perform the object creation, but mark it as "pending" in the Scope until
  // evaluation() time finally marks it as commit().
  void prepare();
  // Commit the object to the Scope, and assign its initial value
  virtual void evaluate();

private:
  bool m_isPrepared;
  std::string m_varname;
  // child 0: the variable being created
  Variable* m_variable;
  // child 1 or 2: the expression that evaluates to the initial value
  // In the absense of an explicit type specifier, this gives us our type
  Expression* m_exp;
  // child 1 if there are 2 children: explicit type specifier
  TypeSpec* m_typeSpec;
  // Type of the new variable.  Provided by m_typeSpec if it exists, otherwise
  // by the type of m_exp if it exists, otherwise stdlib::object.  We only hang
  // on to this temporarily (but require that we have ownership during that
  // time); the new Object will be given ownership at prepare()-time.
  std::auto_ptr<Type> m_type;
  // The new Object that was created.  Set by prepare(), and used to perform
  // the initial value assignment during evaluate().  We do not have ownership,
  // we just hang onto this so we don't have to look it up again.
  Object* m_object;
};

};

#endif // _NewInit_h_
