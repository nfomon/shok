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
    m_root(NULL),
    m_current(NULL) {
  init();
}

AST::~AST() {
  m_log.debug("Destroying AST");
  destroy();
}

void AST::insert(const Token& token) {
  if (!m_root) {
    throw EvalError("Cannot insert token " + token.name + " into evaluator AST with no root token");
  }
  if (!m_current) {
    throw EvalError("Cannot insert token " + token.name + " into evaluator AST with no current token");
  }
  Node* n = Node::MakeNode(m_log, token);
  if (!n) {
    throw EvalError("Failed to make node for token " + token.name + ":" + token.value);
  }
  m_current = Node::insertNode(m_current, n);
}

void AST::reset() {
  m_log.info("Resetting AST. " + print());
  destroy();
  init();
}

void AST::evaluate() {
  if (!m_root) {
    throw EvalError("Cannot evaluate AST with no root node");
  }
  if (!m_current) {
    throw EvalError("Cannot evaluate AST with no current node");
  }
  m_root->reset();
  m_root->reorderOperators();
  m_root->analyzeNode();

  // We only actually run code if m_current has arrived back at the root node,
  // m_root.  This signifies a return to the outer-most (command-line) scope.
  if (m_current != m_root) {
    m_log.debug(" - not at root -- not ready to run");
    return;
  }
  m_root->evaluateNode();
}

string AST::print() const {
  if (m_root) {
    return "<" + m_root->print() + ">";
  }
  return "<>";
}

/* private */

void AST::init() {
  if (m_root || m_current) {
    throw EvalError("Cannot initialize AST overtop of existing or dangling nodes");
  }
  m_root = new RootNode(m_log);
  m_current = m_root;
}

void AST::destroy() {
  if (m_root) {
    delete m_root;
  }
  m_root = NULL;
  m_current = NULL;
}
