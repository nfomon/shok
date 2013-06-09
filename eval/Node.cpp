#include "Node.h"

#include <iostream>
#include <string>
using namespace std;

namespace eval {

Node::Node(const Token& token)
  : name(token.name),
    value(token.value),
    parent(NULL),
    completed(false)
  {
}

Node::~Node() {
  //cout << "Destroying node " << name << endl;
  for (child_vec_iter i = children.begin(); i != children.end(); ++i) {
    delete *i;
  }
}

string Node::print() const {
  string r(name);
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
  for (child_vec_iter i = children.begin(); i != children.end(); ++i) {
    (*i)->evaluate();
  }
  /*
  if ("(" == name && children.size() > 0 && "cmd" == children[0]->name) {
    cerr << "RUN COMMAND: " << print() << endl;
  }
  */
}

};
