// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _Operator_h_
#define _Operator_h_

/* Operators
 *
 * Expression trees are given to us by the parser in a wonky ordering.
 * Consequently at setup() time, we won't have any children.
 * Expression::setup() rearranges us into an Operator tree, then will call
 * analyzeTree() at the root, which will do a top-down reordering of the
 * operators (for operator precedence) followed by a top-down validate() step
 * where all our "real" setup()-type static analysis (read: error checking)
 * will happen.
 *
 * We should split off some of the specific Operators into subclasses of this.
 * But for now it's all here until we're sure of the interface and
 * responsibilities.
 */

#include "Log.h"
#include "Node.h"
#include "RootNode.h"
#include "Token.h"
#include "Type.h"
#include "TypedNode.h"

#include <string>
#include <utility>

namespace eval {

class OperatorParser;

class Operator : public TypedNode {
public:
  friend class OperatorParser;

  static op_precedence Precedence(ARITY arity);

  enum ARITY {
    ARITY_UNKNOWN,
    PREFIX,
    INFIX,
  };

  typedef int op_priority;
  static const NO_PRIORITY = -1;
  enum ASSOC {
    LEFT_ASSOC = 0,
    RIGHT_ASSOC = 1,
  };
  typedef struct {
    op_priority priority;
    ASSOC assoc;
  } op_precedence;

  Operator(Log& log, RootNode*const root, const Token& token)
    : TypedNode(log, root, token),
      isOrderSet(false),
      isValidated(false),
      m_arity(ARITY_UNKNOWN),
      m_left(NULL),
      m_right(NULL) {}

  virtual void setup();

  bool couldBePrefix() const;
  bool couldBeInfix() const;
  void setPrefix();
  void setInfix();
  bool isPrefix() const;
  bool isInfix() const;

  void setupLeft();
  void setupRight();

  virtual void evaluate();

  // Returns the internal method name for this operator, e.g. operator+
  // Returns "" if the operator is not overloadable
  std::string methodName() const;

protected:
  ARITY m_arity;    // set by setPrefix() or setInfix()

private:
  // from TypedNode
  virtual void computeType();

  // Pointers into children; set by OperatorParser before setup().  These
  // should never be freed.
  TypedNode* m_left;
  TypedNode* m_right;
};

};

#endif // _Operator_h_
