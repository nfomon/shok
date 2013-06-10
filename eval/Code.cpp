#include "Code.h"

#include "AST.h"
#include "Log.h"

#include <string>
using std::string;

namespace eval {

Code::Result Code::Operator(Log& log, Node* n) {
  // If there are no children, this operator is just a symbol used on e.g. the
  // command-line.  No evaluation needs to be done.
  if (0 == n->children.size()) return "";
  // Evaluate any children, then perform the maths
  log.error("TODO: maths");
  return "";
}

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

  // We want the user's full command-line the way they wrote it (except we
  // interpret any special symbols like '&', which will have been parsed into a
  // subtree).  However we have annoying tokens like "MINUS" where we just want
  // a '-', so we need to disassemble these back.  If we see a COMMA, it
  // separates actual program arguments, for which a space will be used.
  string cmd;
  for (Node::child_vec_iter i = n->children.begin();
       i != n->children.end(); ++i) {
    (*i)->evaluate();
    cmd += (*i)->cmdText();
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
