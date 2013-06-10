#include "AST.h"

#include "EvalError.h"
#include "Log.h"
#include "Token.h"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <string>
using std::string;
using namespace std;

namespace eval {

/* public */

const std::string ROOT_TOKEN = ":ROOT:";

AST::AST(Log& log)
  : m_log(log),
    m_top(NULL),
    m_current(NULL)
  {
  reset();
}

AST::~AST() {
  m_log.debug("Destroying AST");
  delete m_top;
  m_top = NULL;
  m_current = NULL;
}

void AST::insert(const Token& token) {
  if (!m_current)
    throw EvalError("Cannot insert token " + token.name + " into evaluator AST with no current token");
  Node *n = new Node(m_log, token);
  if ("{" == n->name || "(" == n->name) {
    n->depth = m_current->depth + 1;
    n->parent = m_current;
    m_current->addChild(n);
    m_current = n;    // descend
  } else if ("}" == n->name || ")" == n->name) {
    // m_current should be the open brace/paren to match against
    if (!m_current->parent) {
      if (m_current != m_top) {
        throw EvalError("At token " + token.name + ", found node " + m_current->name + " with no parent but which isn't the root");
      }
      throw EvalError("Cannot move above root node " + m_current->name);
    }
    bool match = ("{" == m_current->name && "}" == n->name) ||
                 ("(" == m_current->name && ")" == n->name);
    if (!match) {
      throw EvalError("Incorrect brace/paren match: '" + boost::lexical_cast<string>(m_current->depth) + "," + m_current->name + "' against '" + n->name + "'");
    }
    m_current->completed = true;      // the brace/paren is done
    for (Node::child_iter i = m_current->children.begin();
         i != m_current->children.end(); ++i) {
      (*i)->completed = true;
    }
    // Parentheses: these are now useless.  We promote the first child (there
    // must be at least one child!) into the paren spot; it is the operator,
    // its children are its operands.  Huzzah!
    if (m_current->name == "(") {
      if (m_current->children.size() < 1) {
        throw EvalError("Empty parens are not allowed. " + print());
      }
      // Steal the child's fields into its parent paren node (m_current)
      Node* op = m_current->children.front();
      m_current->name = op->name;
      m_current->value = op->value;
      delete op;
      m_current->children.pop_front();
    }
    m_current = m_current->parent;
    delete n;   // discard the closing brace/paren
  } else {
    n->depth = m_current->depth + 1;
    m_current->addChild(n);
    n->parent = m_current;
  }
}

void AST::reset() {
  m_log.info("Resetting AST. " + print());
  delete m_top;
  Node *root = new Node(m_log, Token(ROOT_TOKEN));
  m_top = root;
  m_current = root;
}

void AST::evaluate() {
  reorderOperators();
  checkTypes();
  runCode();
}

string AST::print() const {
  if (m_top) {
    return m_top->print();
  }
  return "";
}

/* private */

void AST::reorderOperators() {
  m_log.info("Reordering operators. " + print());
}

void AST::checkTypes() {
  m_log.info("Checking types. " + print());
}

void AST::runCode() {
  m_log.info("Running code. " + print());
  // We only actually run code if m_current has arrived back at the root
  // node, m_top.
  if (m_current != m_top) {
    m_log.debug(" - not at top -- not ready to run");
    return;
  }
  int pop = 0;
  for (Node::child_iter i = m_top->children.begin();
       i != m_top->children.end(); ++i) {
    if (!(*i)->completed) break;
    (*i)->evaluate();
    ++pop;
  }
  while (pop > 0) {
    m_log.debug("Popping: " + m_top->children.front()->name);
    delete m_top->children.front();
    m_top->children.pop_front();
    --pop;
  }
}

};
