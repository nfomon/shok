#include "AST.h"

#include "Token.h"

#include <string>
using std::string;

namespace eval {

/* public */

const std::string ROOT_TOKEN = "";

AST::AST()
  : m_top(NULL),
    m_current(NULL)
  {
  reset();
}

AST::~AST() {
  delete m_top;
  m_top = NULL;
  m_current = NULL;
}

void AST::insert(const Token& token) {
  Node *n = new Node(token);
  if ("}" == n->name || ")" == n->name) {
    if (!m_current->parent) {
      throw ASTError("Cannot move above root node " + m_current->name);
    }
    m_current->addChild(n);
    m_current->completed = true;
    m_current = m_current->parent;
  } else {
    m_current->addChild(n);
    if ("{" == n->name || "(" == n->name) {
      n->parent = m_current;
      m_current = n;
    }
  }
}

void AST::reset() {
  delete m_top;
  Node *root = new Node(Token(ROOT_TOKEN));
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
}

void AST::checkTypes() {
}

void AST::runCode() {
  // We only actually run code if m_current has arrived back at the root
  // node, m_top.
  if (m_current != m_top) return;
  for (Node::child_vec_iter i = m_top->children.begin();
       i != m_top->children.end(); ++i) {
    if ((*i)->completed) {
      (*i)->evaluate();
      //m_top->children.pop_front();
    }
  }
}

};
