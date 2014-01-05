// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Operator_h_
#define _Operator_h_

/* Operators
 *
 * Expression trees are given to us by the parser in a wonky ordering.
 * Consequently, Operators are created and setup carefully by some parent
 * acting as an OperatorParser.  It will use setupLeft() and setupRight()
 * before calling setup().
 *
 * We should split off some of the specific Operators into subclasses of this.
 * But for now it's all here until we're sure of the interface and
 * responsibilities.
 *
 * NOTE:  During setupLeft(), an operator (*ahem* like &) may determine
 * something about its left branch that any random thing that happens on its
 * right side might need to know.  So it can shove this info into some
 * PrattParsingness object, and this data should get passed down the initNode()
 * of everything we parse until we bubble up and finally call the setupRight()
 * on this operator (followed by its own setup(), naturally!).  We can do that
 * (with the refactoring involved) when we implement &.
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

  enum ARITY {
    ARITY_UNKNOWN,
    PREFIX,
    INFIX
  };

  typedef int op_priority;
  static const int NO_PRIORITY = -1;
  enum ASSOC {
    LEFT_ASSOC = 0,
    RIGHT_ASSOC = 1
  };
  typedef struct {
    op_priority priority;
    ASSOC assoc;
  } op_precedence;

  Operator(Log& log, RootNode*const root, const Token& token)
    : TypedNode(log, root, token),
      m_arity(ARITY_UNKNOWN),
      m_left(NULL),
      m_right(NULL),
      isPartiallySetup(false) {}

  virtual void setup();

  bool couldBePrefix() const;
  bool couldBeInfix() const;
  void setPrefix();
  void setInfix();
  bool isPrefix() const;
  bool isInfix() const;
  op_precedence precedence(ARITY arity);

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

  // Pointers into children; set by setupLeft() and setupRight().  These should
  // never be freed.
  TypedNode* m_left;
  TypedNode* m_right;

protected:
  // set by setupLeft() or setupRight(), whichever is last for this operator
  bool isPartiallySetup;
};

}

#endif // _Operator_h_
