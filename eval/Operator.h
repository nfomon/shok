// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _Operator_h_
#define _Operator_h_

/* Operators
 *
 * Operator trees are given to us by the parser in a wonky ordering.
 * Consequently at setup() time, we can't trust our children.
 * Expression::setup() wraps the top of the Operator tree, and will call
 * analyzeTree() at the root, which will do a top-down reordering of the
 * operators followed by a top-down validate() step where all our "real"
 * setup()-type static analysis (read: error checking) will happen.
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

namespace eval {

class Operator : public TypedNode {
public:
  Operator(Log& log, RootNode*const root, const Token& token)
    : TypedNode(log, root, token),
      isReordered(false),
      isValidated(false),
      isUnary(false),
      isBinary(false),
      left(NULL),
      right(NULL) {}

  virtual void setup();

  // Static analysis of a whole operator tree.  Reorders operators to account
  // for operator precedence rules, and validates the operators bottom-up.
  // Called by Expression::setup(), which wraps the top of the operator tree.
  void analyzeTree();

  virtual void evaluate();

  // Returns the method name for this operator, e.g. operator+
  // Returns "" if the operator is not overloadable
  std::string methodName() const;

protected:
  virtual void validate();
  virtual int priority() const;

  bool isReordered;
  bool isValidated;
  bool isUnary;
  bool isBinary;

private:
  // Reorder tree of operators for our priority-based precedence rules,
  // starting at this.  Called by analyzeTree().
  void reorderOperatorTree();
  // Validate tree of operators, starting at this.  Called by analyzeTree().
  // This late syntactic analysis is kickstarted by the parent Expression.
  void validateOperatorTree();

  // from TypedNode
  virtual void computeType();

  // Pointers into children; set by validate(), should not be freed
  TypedNode* left;
  TypedNode* right;
};

};

#endif // _Operator_h_
