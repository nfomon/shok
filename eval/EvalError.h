// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _EvalError_h_
#define _EvalError_h_

/* Evaluation errors.  Used for both internal evaluator-code errors as
 * well as errors regarding the input AST, or errors in the generated
 * code.
 *
 * Anywhere in the code that an error occurs, an EvalError is thrown.
 * Node::insertNode() can catch some of them, deem them "recoverable",
 * and throws a RecoveredError instead.  The RecoveredError contains a
 * good known position along the AST which we should use as the new
 * current location.  AST::insertNode() catches this and uses this
 * known position, and then re-throws the RecoveredError again.
 *
 * The top-level REPL (main) catches both types of errors.  If it's
 * not a recovered error, then it resets the AST (clears any
 * unevaluated code and returns us to the root scope).
 */

#include <stdexcept>

namespace eval {

class Node;

class EvalError : public std::runtime_error {
public:
  EvalError(const std::string& what) : std::runtime_error(what) {}
};

class RecoveredError : public std::runtime_error {
public:
  RecoveredError(EvalError& originalError, Node* recoveredPosition)
    : std::runtime_error(originalError.what()),
      m_recoveredPosition(recoveredPosition) {}

  Node* getRecoveredPosition() const { return m_recoveredPosition; }

private:
  Node* m_recoveredPosition;
};

};

#endif // _EvalError_h_
