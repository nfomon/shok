#include "Command.h"

#include "Node.h"

#include <string>
using std::string;

using namespace eval;

void Command::complete() {
  m_isComplete = true;
}

void Command::evaluate() {
  string s;
  for (Node::child_iter i = children.begin(); i != children.end(); ++i) {
    (*i)->evaluate();
    s += (*i)->cmdText();
  }
  log.info("RUN CMD: " + s);
}
