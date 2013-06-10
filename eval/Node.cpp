#include "Node.h"

#include "AST.h"
#include "Code.h"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <string>
using namespace std;

namespace eval {

class ASTError;

Node::Node(Log& log, const Token& token)
  : log(log),
    completed(false),
    depth(0),
    name(token.name),
    value(token.value),
    parent(NULL)
  {
}

Node::~Node() {
  log.debug("Destroying node " + name);
  for (child_vec_iter i = children.begin(); i != children.end(); ++i) {
    delete *i;
  }
}

string Node::print() const {
  string r(boost::lexical_cast<string>(depth) + "_" + name);
  if (value.length() > 0) {
    r += ":" + value;
  }
  for (child_vec_iter i = children.begin(); i != children.end(); ++i) {
    if (r != "") r += " ";
    r += (*i)->print();
  }
  return r;
}

void Node::addChild(Node* child) {
  children.push_back(child);
}

void Node::evaluate() {
  log.debug("Evaluating node: " + name);
  // Don't necessarily evaluate the children immediately.  To allow for, e.g.,
  // short-circuiting.  Let this node decide.
  if (!completed) throw ASTError("Cannot evaluate incomplete node " + name);
  if ("{" == name) {
    Code::Block(log, this);
  } else if ("cmd" == name) {
    Code::Cmd(log, this);
  } else if ("new" == name) {
    Code::New(log, this);
  }
}

};
