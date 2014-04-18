// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _CompileError_h_
#define _CompileError_h_

/* Compilation errors.  Used for both internal compiler-code errors as well as
 * errors regarding the input AST, or errors in the generated code.
 *
 * Anywhere in the code that an error occurs, a CompileError is thrown.
 * Node::insertNode() can catch some of them, deem them "recoverable", and
 * throws a RecoveredError instead.  The RecoveredError contains a good known
 * position along the AST which we should use as the new current location.
 * AST::insertNode() catches this and uses this known position, and then
 * re-throws the RecoveredError again.
 *
 * The top-level REPL (main) catches both types of errors.  If it's not a
 * recovered error, then it resets the AST (clears any uncompiled code and
 * returns us to the root scope).
 */

#include "util/Log.h"

#include <stdexcept>

namespace compiler {

class Node;

class CompileError : public std::runtime_error {
public:
  CompileError(const std::string& what) : std::runtime_error(what) {}
  CompileError(Log& log, const std::string& what) : std::runtime_error(what) {
    log.error(what);
  }
};

class RecoveredError : public std::runtime_error {
public:
  RecoveredError(CompileError& originalError, Node* recoveredPosition)
    : std::runtime_error(originalError.what()),
      m_recoveredPosition(recoveredPosition) {}

  Node* getRecoveredPosition() const { return m_recoveredPosition; }

private:
  Node* m_recoveredPosition;
};

}

#endif // _CompileError_h_
