#include "Code.h"

#include "AST.h"
#include "Log.h"

#include <string>
using std::string;

namespace eval {

Code::Result Code::Block(Log& log, Node* n) {
  log.debug("Code: Block");
  // Evaluate all children in sequence
  for (Node::child_vec_iter i = n->children.begin();
       i != n->children.end(); ++i) {
    (*i)->evaluate();
  }
  return "";
}

Code::Result Code::Cmd(Log& log, Node* n) {
  log.debug("Code: Cmd");
  string cmd = n->children.front()->name + ":" + n->children.front()->value;
  for (Node::child_vec_iter i = n->children.begin() + 1;
       i != n->children.end(); ++i) {
    cmd += string("__") + (*i)->name + ":" + (*i)->value;
  }
  log.info("RUN CMD: " + cmd);
  return "";
}

Code::Result Code::New(Log& log, Node* n) {
  log.debug("Code: New");
  if (n->children.size() < 1) throw ASTError("new must have at least 1 child");
  return "";
}

};
