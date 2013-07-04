#include "Node.h"

#include "Block.h"
#include "Brace.h"
#include "Command.h"
#include "CommandFragment.h"
#include "EvalError.h"
#include "ExpressionBlock.h"
#include "Log.h"
#include "New.h"
#include "NewInit.h"
#include "Operator.h"
#include "Variable.h"

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
    return new Variable(log, t);
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
  if ("expblock" == t.name)
    return new ExpressionBlock(log, t);
  if ("new" == t.name)
    return new New(log, t);
  if ("init" == t.name)
    return new NewInit(log, t);
  throw EvalError("Unsupported token " + t.print());
  return NULL;    // guard
}

/* Members */

Node::Node(Log& log, const Token& token)
  : log(log),
    name(token.name),
    value(token.value),
    m_isSetup(false),
    m_isComplete(false),
    parent(NULL),
    block(NULL) {
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

void Node::setup() {
  if (m_isSetup) {
    throw EvalError("Cannot setup already-setup Node " + print());
  }
  if (!parent) {
    throw EvalError("Cannot setup Node " + print() + " with no parent");
  }
  Block* b = dynamic_cast<Block*>(parent);
  block = b ? b : parent->block;
}

void Node::setupAndCompleteAsParent() {
  // The node's grandchildren should all already be complete.
  // We setup the node, then setup its children.
  // We complete the children, then complete the node.
  setup();
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    for (child_iter j = (*i)->children.begin();
         j != (*i)->children.end(); ++j) {
      if (!(*j)->isComplete()) {
        throw EvalError("Node " + print() + " child " + (*i)->print() +
                        " grandchild " + (*j)->print() + " did not complete");
      }
    }
    (*i)->setup();
    if (!(*i)->isComplete()) {
      (*i)->complete();
    }
    if (!(*i)->isComplete()) {
      throw EvalError("Node " + print() + " child " + (*i)->print() +
                      " did not complete");
    }
  }
  if (isComplete()) {
    throw EvalError("Node " + print() + " is complete too early");
  }
  complete();
  if (!isComplete()) {
    throw EvalError("Node " + print() + " did not complete");
  }
}

void Node::reorderOperators() {
  if (!isSetup() || !isComplete()) {
    throw EvalError("Node " + print() + " cannot be reordered until setup and complete");
  }
  if (isReordered()) {
    throw EvalError("Node " + print() + " already has reordered operators");
  }
  switch (children.size()) {
    case 0: return;
    // Operator with a single operator child: error, unsupported (it's unclear
    // to me how these should be reordered, since I don't think they exist)
    case 1: {
      Node* child = children.at(0);
      child->reorderOperators();
      Operator* op = dynamic_cast<Operator*>(this);
      if (!op) return;
      Operator* childOp = dynamic_cast<Operator*>(child);
      if (childOp) {
        throw EvalError("Unclear how to reorder unary operator with operator child");
      }
    } return;
    // Left child is fine.  If right child has lower or equal priority than us,
    // shuffle it around, and *then* recurse down the right child.
    case 2: {
      Node* left = children.at(0);
      Node* right = children.at(1);
      left->reorderOperators();   // Optimization: we can probably skip this if
                                  // it's a left we switcheroo'd! (maybe?)
      Operator* op = dynamic_cast<Operator*>(this);
      Operator* rightOp = dynamic_cast<Operator*>(right);
      if (!op || !rightOp ||
          rightOp->children.size() != 2 ||
          rightOp->priority() > op->priority()) {
        right->reorderOperators();
        return;
      }
      if (!op->parent || !rightOp->parent) {
        throw EvalError("Someone's parent is deficient");
      }
      op->children.pop_back();
      op->children.push_back(rightOp->children.at(0));
      rightOp->parent = op->parent;
      op->parent = rightOp;
      rightOp->children.pop_front();
      rightOp->children.push_front(op);
      for (child_mod_iter i = rightOp->parent->children.begin();
           i != rightOp->parent->children.end(); ++i) {
        if (*i == op) {
          *i = rightOp;
          break;
        }
      }
      rightOp->reorderOperators();
    } return;
    default: {
      for (child_iter i = children.begin(); i != children.end(); ++i) {
        (*i)->reorderOperators();
      }
    } return;
  }
}

void Node::staticAnalysis() {
  // Analyze nodes children-first
  if (!isSetup() || !isComplete() || !isReordered()) {
    throw EvalError("Node " + print() + " cannot do static analysis until setup, complete, reordered");
  }
  if (isAnalyzed()) {
    throw EvalError("Node " + print() + " has already done static analysis");
  }
  for (child_iter i = children.begin(); i != children.end(); ++i) {   
    (*i)->staticAnalysis();
  }
  analyze();
}

Node::operator std::string() const {
  return name;
}
