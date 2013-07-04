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
  Brace* brace = dynamic_cast<Brace*>(n);

  // Neither an open nor a closing brace; add as a child of m_current
  if (!brace) {
    n->parent = m_current;
    m_current->addChild(n);

  // Open brace: descend into m_current; new nodes will be its children
  } else if (brace->isOpen()) {
    n->parent = m_current;
    m_current->addChild(n);
    m_current = n;    // descend

  // Closing brace: ensure it matches with the open brace (m_current), then
  // ascend our focus up.
  //
  // When parentheses are matched, they will be eliminated from the AST since
  // they represent nothing.  Instead, their first child (operator) will take
  // over the "parent" spot; its children (operands) will remain as the
  // operator's children.
  } else if (!brace->isOpen()) {
    // m_current should be the open brace/paren to match against
    if (!m_current->parent) {
      if (m_current != m_root) {
        throw EvalError("At token " + token.name + ", found node " + m_current->name + " with no parent but which isn't the root");
      }
      throw EvalError("Cannot move above root node " + m_current->name);
    }

    Brace* open = dynamic_cast<Brace*>(m_current);
    if (!open) {
      throw EvalError("Found closing brace " + brace->name + " but parent " + m_current->name + " is not an open brace");
    }
    if (!open->matchesCloseBrace(brace)) {
      throw EvalError("Incorrect brace/paren match: '" + open->name + "' against '" + n->name + "'");
    }

    // Parentheses: these are now useless.  We promote the first child (there
    // must be at least one child!) into the parent (paren) spot; it is the
    // operator, its children are its operands.  Huzzah!
    if (open->isIrrelevant()) {
      // Extract the first child of the open brace; it is the new "operator"
      if (open->children.size() < 1) {
        throw EvalError("Empty parens in the AST are not allowed");
      }
      Node* op = open->children.front();    // "operator" becomes the parent
      open->children.pop_front();
      op->parent = open->parent;
      // a '({' cannot appear in the input.  Note that parentheses in
      // expressions are not given to us as a bare '(' brace.
      if (op->children.size() != 0) {
        throw EvalError("Cannot escalate child " + op->name + " that has " + boost::lexical_cast<string>(op->children.size()) + " > 0 children");
      }
      op->children = open->children;
      open->children.clear();
      // Replace op's children's parent links from open to op
      for (Node::child_iter i = op->children.begin();
           i != op->children.end(); ++i) {
        (*i)->parent = op;
      }
      // Replace parent's child of 'open' with 'op'
      for (Node::child_mod_iter i = op->parent->children.begin();
           i != op->parent->children.end(); ++i) {
        if (*i == open) {
          *i = op;
          break;    // a node must only appear once in the AST
        }
      }
      m_log.debug("Deleting open: " + open->print());
      delete open;
      op->setupAndCompleteAsParent();
    } else {
      open->setupAndCompleteAsParent();
    }
    m_current = m_current->parent;    // ascend
    m_log.debug("Deleting n: " + n->print());
    delete n;   // always discard the closing brace/paren
  }
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
  m_root->reorderOperators();
  m_root->staticAnalysis();
  runCode();
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

void AST::runCode() {
  m_log.info("Running code. " + print());
  // We only actually run code if m_current has arrived back at the root node,
  // m_root.  This signifies a return to the outer-most (command-line) scope.
  if (!m_root) {
    throw EvalError("Cannot evaluate AST -- no root node");
  }
  if (!m_current) {
    throw EvalError("Cannot evaluate AST -- no current node");
  }
  if (m_current != m_root) {
    m_log.debug(" - not at root -- not ready to run");
    return;
  }
  m_root->evaluate();
}
