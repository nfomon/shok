#include "Node.h"

#include "Code.h"
#include "EvalError.h"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <string>
using namespace std;

namespace eval {

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
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    delete *i;
  }
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

void Node::addChild(Node* child) {
  children.push_back(child);
}

void Node::evaluate() {
  log.debug("Evaluating node: " + name);
  // Don't necessarily evaluate the children immediately.  To allow for, e.g.,
  // short-circuiting.  Let this node decide.
  if (!completed) throw EvalError("Cannot evaluate incomplete node " + name);
  if (isPrimitive()) return;
  if (isOperator()) {
    Code::Operator(log, this);
  } else if ("{" == name) {
    Code::Block(log, this);
  } else if ("cmd" == name) {
    Code::Cmd(log, this);
  } else if ("new" == name) {
    Code::New(log, this);
  } else {
    log.warning("Don't know how to evaluate node " + name + "; skipping");
  }
}

bool Node::isPrimitive() {
  if ("," == name ||
      "ID" == name ||
      "INT" == name ||
      "FIXED" == name) {
    return true;
  }
  return false;
}

bool Node::isOperator() {
  if ("PLUS" == name ||
      "MINUS" == name ||
      "MULT" == name ||
      "DIV" == name) {
    return true;
  }
  return false;
}

string Node::cmdText() {
  if ("," == name) {
    return " ";
  } else if ("MINUS" == name) {
    return "-";
  } else if ("ID" == name ||
             "INT" == name ||
             "FIXED" == name) {
    if ("" == value) {
      throw EvalError("Token " + name + " cannot have blank value");
    }
    return value;
  }
}

};
