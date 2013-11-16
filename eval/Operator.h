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

class Operator : public TypedNode {
public:
  static bool CouldBeUnary(const std::string& opName);
  static bool CouldBeBinary(const std::string& opName);

  typedef int op_priority;
  enum ASSOC {
    LEFT_ASSOC = 0,
    RIGHT_ASSOC = 1,
  };
  typedef struct {
    op_priority priority;
    ASSOC assoc;
  } op_prec;

  Operator(Log& log, RootNode*const root, const Token& token)
    : TypedNode(log, root, token),
      isOrderSet(false),
      isValidated(false),
      isUnary(false),
      isBinary(false),
      m_left(NULL),
      m_right(NULL) {}

  virtual void setup();
  virtual op_prec precedence() const;

  // Sets whether this is a unary or binary operator.  Called by
  // Node::MakeOperatorTree().
  void setUnary();
  void setBinary();

  // Static analysis of a whole operator tree.  Called by
  // Node::MakeOperatorTree().
  void analyzeTree();

  virtual void evaluate();

  // Returns the internal method name for this operator, e.g. operator+
  // Returns "" if the operator is not overloadable
  std::string methodName() const;

protected:
  // Setup validation, called by validateOperatorTree().
  virtual void validate();

  bool isOrderSet;  // set by setUnary() or setBinary()
  bool isValidated;
  bool isUnary;     // set by analysisSetup()
  bool isBinary;    // set by analysisSetup()

private:
  // Validate tree of operators, starting at this.  Called by analyzeTree().
  void validateOperatorTree();

  // from TypedNode
  virtual void computeType();

  // Pointers into children; set by validate(), should not be freed
  TypedNode* m_left;
  TypedNode* m_right;
};

};

#endif // _Operator_h_
