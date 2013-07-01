#include "Node.h"

#include "Block.h"
#include "Brace.h"
#include "Command.h"
#include "CommandFragment.h"
#include "EvalError.h"
#include "ExpressionBlock.h"
#include "Identifier.h"
#include "Log.h"
#include "Operator.h"

#include <boost/lexical_cast.hpp>

#include <string>
using std::string;

using namespace eval;

/* Statics */

Node* Node::MakeNode(Log& log, const Token& t) {
  if ("[" == t.name)
    return new Command(log, t);
  if ("(" == t.name)
    return new Brace(log, t, true);
  if ("{" == t.name)
    return new Block(log, t);
  if ("]" == t.name ||
      ")" == t.name ||
      "}" == t.name)
    return new Brace(log, t, false);
  if ("cmd" == t.name)
    return new CommandFragment(log, t);
  if ("ID" == t.name)
    return new Identifier(log, t);
  if ("PLUS" == t.name ||
      "MINUS" == t.name ||
      "STAR" == t.name ||
      "SLASH" == t.name ||
      "PERCENT" == t.name ||
      "CARAT" == t.name ||
      "PIPE" == t.name ||
      "AMP" == t.name ||
      "TILDE" == t.name ||
      "DOUBLETILDE" == t.name)
    return new Operator(log, t);
  if ("exp" == t.name)
    return new ExpressionBlock(log, t);
  throw EvalError("Unsupported token " + t.print());
  return NULL;    // guard
}

/* Members */

Node::Node(Log& log, const Token& token)
  : log(log),
    name(token.name),
    value(token.value),
    m_isComplete(false),
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
  string r = name;
  if (value.length() > 0) {
    r += ":" + value;
  }
  if (children.size() > 0) {
    r += "(";
    for (child_iter i = children.begin(); i != children.end(); ++i) {
      if (i != children.begin()) r += " ";
      r += (*i)->print();
    }
    r += ")";
  }
  return r;
}

Node::operator std::string() const {
  return name;
}
