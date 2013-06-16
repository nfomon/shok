#include "Node.h"

#include "Block.h"
#include "Brace.h"
#include "Command.h"
#include "EvalError.h"
#include "Identifier.h"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <string>
using namespace std;

namespace eval {

/* Statics */

Node* Node::MakeNode(Log& log, const Token& t) {
  if ("ID" == t.name)
    return new Identifier(log, t);
  if ("cmd" == t.name)
    return new Command(log, t);
  if ("{" == t.name)
    return new Block(log, t);
  if ("(" == t.name)
    return new Brace(log, t, true);
  if (")" == t.name ||
      "}" == t.name)
    return new Brace(log, t, false);
/*
  } else if ("+" == t.name ||
             "-" == t.name ||
             "*" == t.name ||
             "/" == t.name) {
    return new Operator(log, t);
*/
  throw EvalError("Unsupported token " + t.print());
  return NULL;    // guard
}

/* Members */

Node::Node(Log& log, const Token& token)
  : log(log),
    name(token.name),
    value(token.value),
    m_isComplete(false),
    depth(0),
    parent(NULL) {
}

Node::~Node() {
  log.debug("Destroying node " + name);
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    delete *i;
  }
}

void Node::addChild(Node* child) {
  children.push_back(child);
}

bool Node::isComplete() const {
  return m_isComplete;
}

string Node::print() const {
  string r(boost::lexical_cast<string>(depth) + "_" + name);
  if (value.length() > 0) {
    r += ":" + value;
  }
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    if (r != "") r += " ";
    r += (*i)->print();
  }
  return r;
}

string Node::cmdText() const {
  throw EvalError("Node " + name + " has no command-line text");
}


/* Root Node */
RootNode::RootNode(Log& log)
  : Node(log, Token(":ROOT:")) {
}

void RootNode::complete() {
  throw EvalError("Cannot complete the root node");
}

void RootNode::evaluate() {
  log.debug("Evaluating root node");
  if (children.size() < 1) return;

  // Error if anyone other than the last child is incomplete
  for (child_iter i = children.begin(); i != children.end()-1; ++i) {
    if (!(*i)->isComplete() && i != children.end()-1) {
      throw EvalError("RootNode: found multiple incomplete children");
    }
  }
  int done = 0;
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    if ((*i)->isComplete()) {
      (*i)->evaluate();
      ++done;
    }
  }
  while (done > 0) {
    delete children.front();
    children.pop_front();
    --done;
  }
}

};
