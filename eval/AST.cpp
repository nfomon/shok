// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#include "AST.h"

#include "Brace.h"
#include "EvalError.h"
#include "Log.h"
#include "Token.h"
#include "RootNode.h"
#include "Operator.h"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <string>
using std::string;

using namespace eval;

/* public */

AST::AST(Log& log)
  : m_log(log),
    m_root(log),
    m_current(&m_root) {
}

AST::~AST() {
}

void AST::reset() {
  m_log.info("Resetting AST. " + print());
  m_current = &m_root;
  m_root.reset();
}

void AST::insert(const Token& token) {
  Node* n = Node::MakeNode(m_log, &m_root, token);
  if (!n) {
    throw EvalError("Failed to make node for token " + token.name + ":" + token.value);
  }
  try {
    m_current = Node::insertNode(m_log, m_current, n);
  } catch (RecoveredError& e) {
    m_current = e.getRecoveredPosition();
    if (!m_current) {
      throw EvalError(string("Attempting to recover from error '") + e.what() + "' returned a deficient current node");
    }
    throw;
  }
  if (!m_current) {
    throw EvalError("Inserting node " + n->print() + " returned a deficient current node");
  }
  m_log.debug("AST: " + print());
}

void AST::evaluate() {
  m_root.prepare();

  // We only actually run code if m_current has arrived back at the root node,
  // m_root.  This signifies a return to the outer-most (command-line) scope.
  if (m_current != &m_root) {
    m_log.debug(" - not at root -- not ready to run");
    return;
  }
  m_root.evaluateNode();
}

string AST::print() const {
  return "<" + m_root.print() + ">";
}
