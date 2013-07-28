// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _Operator_h_
#define _Operator_h_

/* Operator */

#include "Log.h"
#include "Node.h"
#include "RootNode.h"
#include "Token.h"

#include <string>

namespace eval {

class Operator : public Node {
public:
  Operator(Log& log, RootNode*const root, const Token& token)
    : Node(log, root, token),
      isReordered(false),
      isValidated(false),
      isUnary(false),
      isBinary(false) {}

  virtual void setup();

  // Static analysis of a whole operator tree.  Reorders operators to account
  // for operator precedence rules, and validates the operators bottom-up.
  // Called by Expression::setup(), which wraps the top of the operator tree.
  void analyzeTree();

  virtual void evaluate();

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
};

};

#endif // _Operator_h_
