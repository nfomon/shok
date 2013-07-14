// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

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
#include "RootNode.h"
#include "Variable.h"

#include <boost/lexical_cast.hpp>

#include <string>
using std::string;

using namespace eval;

/* Statics */

Node* Node::MakeNode(Log& log, RootNode*const root, const Token& t) {
  if ("[" == t.name)
    return new Command(log, root, t);
  if ("(" == t.name)
    return new Brace(log, root, t, true);
  if ("{" == t.name)
    return new Block(log, root, t);
  if ("]" == t.name ||
      ")" == t.name ||
      "}" == t.name)
    return new Brace(log, root, t, false);
  if ("cmd" == t.name)
    return new CommandFragment(log, root, t);
  if ("ID" == t.name)
    return new Variable(log, root, t);
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
    return new Operator(log, root, t);
  if ("exp" == t.name)
    return new ExpressionBlock(log, root, t);
  if ("new" == t.name)
    return new New(log, root, t);
  if ("init" == t.name)
    return new NewInit(log, root, t);
  throw EvalError("Unsupported token " + t.print());
  return NULL;    // guard
}

/* Members */

Node::Node(Log& log, RootNode*const root, const Token& token)
  : log(log),
    root(root),
    name(token.name),
    value(token.value),
    isSetup(false),
    isReordered(false),
    isAnalyzed(false),
    isEvaluated(false),
    parent(NULL),
    parentScope(NULL) {
}

Node::~Node() {
  log.debug("Destroying node " + name);
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    delete *i;
  }
}

Node* Node::insertNode(Log& log, Node* current, Node* n) {
  if (!current || !n) {
    throw EvalError("NULL nodes provided to Node::insertNode()");
  }
  Brace* brace = dynamic_cast<Brace*>(n);

  // Neither an open nor a closing brace; add as a child of current
  if (!brace) {
    n->parent = current;
    current->addChild(n);
    return current;   // stay
  }

  // Open brace: descend into current; new nodes will be its children
  if (brace->isOpen()) {
    n->parent = current;
    current->addChild(n);
    return n;         // descend
  }

  // Closing brace: ensure it matches with the open brace (current), then
  // ascend our focus up.
  //
  // When parentheses are matched, they will be eliminated from the AST since
  // they represent nothing.  Instead, their first child (operator) will take
  // over the "parent" spot; its children (operands) will remain as the
  // operator's children.
  // current should be the open brace/paren to match against
  if (!current->parent) {
    throw EvalError("Cannot move above root node " + current->name);
  }

  Brace* open = dynamic_cast<Brace*>(current);
  if (!open) {
    throw EvalError("Found closing brace " + brace->name + " but its parent " + current->name + " is not an open brace");
  }
  if (!open->matchesCloseBrace(brace)) {
    throw EvalError("Incorrect brace/paren match: '" + open->name + "' against '" + n->name + "'");
  }
  Node* parent = current->parent;

  // Parentheses: these are now useless.  We promote the first child (there
  // must be at least one child!) into the parent (paren) spot; it is the
  // operator, its children are its operands.  Huzzah!
  if (open->isIrrelevant()) {
    // Extract the first child of the open brace; it is the new "operator"
    if (open->children.size() < 1) {
      throw EvalError("Empty parens in the AST are not allowed");
    }
    Node* op = open->children.front();    // "operator" becomes the parent
    open->children.pop_front();
    op->parent = open->parent;
    // a '({' cannot appear in the input.  Note that actual parentheses in
    // expressions are not given to us as a bare '(' brace.
    if (op->children.size() != 0) {
      throw EvalError("Cannot escalate child " + op->name + " that has " + boost::lexical_cast<string>(op->children.size()) + " > 0 children");
    }
    op->children = open->children;
    open->children.clear();   // Clear open's children so they're not deleted
    // Replace op's children's parent links from open to op
    for (Node::child_iter i = op->children.begin();
         i != op->children.end(); ++i) {
      (*i)->parent = op;
    }
    // Replace parent's child of 'open' with 'op'
    for (Node::child_mod_iter i = op->parent->children.begin();
         i != op->parent->children.end(); ++i) {
      if (*i == open) {
        *i = op;
        break;    // a node must only appear once in the AST
      }
    }
    delete open;
    op->setupAsParent();
  } else {
    open->setupAsParent();
  }
  delete n;   // always discard the closing brace/paren
  return parent;    // ascend
}

// Called only on nodes that are understood to be parents.
// We setupNode() the nodes children-first.
void Node::setupAsParent() {
  // The node's grandchildren should all already be setup.
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    (*i)->setupNode();
  }
  setupNode();
  log.debug("Setup node " + print());
}

void Node::setupNode() {
  if (isSetup) return;
  if (!parent) {
    throw EvalError("Cannot setup Node " + print() + " with no parent");
  }
  log.debug(" - setting up node " + print());
  setup();
  isSetup = true;
}

void Node::reorderOperators() {
  log.debug("reorder " + print());
  if (isReordered) return;
  if (!isSetup) {
    // An immediate child of the root can skip reordering if it's not setup
    if (root == parent) return;
    throw EvalError("Node " + print() + " cannot be reordered until it's setup");
  }
  switch (children.size()) {
    case 0: break;
    // Operator with a single operator child: error, unsupported (it's unclear
    // to me how these should be reordered, since I don't think they exist)
    case 1: {
      Node* child = children.at(0);
      child->reorderOperators();
      Operator* op = dynamic_cast<Operator*>(this);
      if (!op) break;
      Operator* childOp = dynamic_cast<Operator*>(child);
      if (childOp) {
        throw EvalError("Unclear how to reorder unary operator with operator child");
      }
    } break;
    // Left child is fine.  If right child has lower or equal priority than us,
    // shuffle it around, and *then* recurse down the right child.
    case 2: {
      Node* left = children.at(0);
      Node* right = children.at(1);
      left->reorderOperators();
      Operator* op = dynamic_cast<Operator*>(this);
      Operator* rightOp = dynamic_cast<Operator*>(right);
      if (!op || !rightOp ||
          rightOp->children.size() != 2 ||
          rightOp->priority() > op->priority()) {
        right->reorderOperators();
        break;
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
    } break;
    default: {
      for (child_iter i = children.begin(); i != children.end(); ++i) {
        (*i)->reorderOperators();
      }
    } break;
  }
  log.debug(" - reordered node " + print());
  isReordered = true;
}

void Node::analyzeNode() {
  if (isAnalyzed) return;
  if (!isSetup || !isReordered) {
    // An immediate child of the root can skip analysis if it's not setup
    if (root == parent) return;
    throw EvalError("Node " + print() + " cannot do static analysis until setup and reordered");
  }

  // Assign parent scopes from parent downwards
  if (parent) {
    parentScope = parent->getScope();
    if (!parentScope) parentScope = parent->parentScope;
    if (parentScope) {
      log.debug("Gave parentScope to " + print());
    } else {
      log.debug("Did not give parentScope to " + print());
    }
  }

  // Parent-first static analysis
  analyzeDown();

  for (child_iter i = children.begin(); i != children.end(); ++i) {   
    (*i)->analyzeNode();
  }

  // Child-first static analysis
  log.debug(" - analyzing node " + print());
  analyzeUp();
  isAnalyzed = true;
}

void Node::evaluateNode() {
  if (isEvaluated) {
    throw EvalError("Node " + print() + " has already been evaluated");
  }
  if (!isSetup || !isReordered || !isAnalyzed) {
    // An immediate child of the root can skip evaluation if it's not setup
    if (root == parent) return;
    throw EvalError("Node " + print() + " cannot be evaluated until setup, reordered, analyzed");
  }
  // Evaluate nodes children-first
  for (child_iter i = children.begin(); i != children.end(); ++i) {   
    (*i)->evaluateNode();
  }
  log.debug(" - evaluating node " + print());
  evaluate();
  isEvaluated = true;
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

/* protected */

void Node::addChild(Node* child) {
  children.push_back(child);
}
