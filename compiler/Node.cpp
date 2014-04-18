// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Node.h"

#include "Arg.h"
#include "Args.h"
#include "Block.h"
#include "Brace.h"
#include "Command.h"
#include "CommandFragment.h"
#include "CompileError.h"
#include "Expression.h"
#include "Function.h"
#include "Identifier.h"
#include "IsVar.h"
#include "New.h"
#include "NewInit.h"
#include "ObjectLiteral.h"
#include "Operator.h"
#include "OperatorParser.h"
#include "ProcCall.h"
#include "Returns.h"
#include "RootNode.h"
#include "TypeSpec.h"
#include "Variable.h"

#include "util/Log.h"

#include <boost/lexical_cast.hpp>

#include <string>
using std::string;

using namespace compiler;

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
    return new Identifier(log, root, t);
  if ("var" == t.name)
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
    return new Expression(log, root, t);
  if ("new" == t.name)
    return new New(log, root, t);
  if ("init" == t.name)
    return new NewInit(log, root, t);
  if ("type" == t.name)
    return new TypeSpec(log, root, t);
  if ("call" == t.name)
    return new ProcCall(log, root, t);
  if ("isvar" == t.name)
    return new IsVar(log, root, t);
  if ("object" == t.name)
    return new ObjectLiteral(log, root, t);
  if ("func" == t.name)
    return new Function(log, root, t);
  if ("args" == t.name)
    return new Args(log, root, t);
  if ("arg" == t.name)
    return new Arg(log, root, t);
  if ("returns" == t.name)
    return new Returns(log, root, t);
  throw CompileError("Unsupported token " + t.print());
  return NULL;    // guard
}

Node* Node::InsertNode(Log& log, Node* current, Node* n) {
  if (!current || !n) {
    throw CompileError("NULL nodes provided to Node::InsertNode()");
  }
  Brace* brace = dynamic_cast<Brace*>(n);

  // Neither an open nor a closing brace; add as a child of current
  if (!brace) {
    // Some nodes employ Pratt parsing, which allows children nodes to be
    // setup() before the parent is complete.  Check if that's the case here,
    // i.e. if current is an OperatorParser.  Then:
    //  - Don't insert the current node.
    //  - Instead, call oP->insertNode(), which will own the inserted node.
    //  - When we hit the closing brace of current, we must call
    //    oP->finalizeParse().  This gives us the root of the operator-tree,
    //    which must become the first and only child of current.
    // Here we deal with non-brace children of an oP.  But we could also have
    // children that start with opening braces.  We should do everything with
    // them, then once the irrelevant(I think always) closing brace matches, we
    // insertNode() it into the oP.
    n->initScopeNode(current);    // Whether we have an oP or not.
    OperatorParser* oP = NULL;
    if (current->children.size() > 0) {
      oP = dynamic_cast<OperatorParser*>(current->children.at(0));
      if (oP) {
        oP->insertNode(n);
      }
    }
    if (!oP) {
      n->parent = current;
      current->addChild(n);
      if (current->children.size() > 1) {
        current->children.at(0)->initChild(n);
      }
    }
    return current;   // stay
  }

  // Open brace: descend into current; new nodes will be its children
  // Note the hack: for the { child of Function
  if (brace->isOpen()) {
    n->parent = current;
    current->addChild(n);
    if (!brace->isIrrelevant() && current->children.size() > 1) {
      current->children.at(0)->initChild(n);
    }
    n->initScopeNode(current);
    return n;         // descend
  }

  // Closing brace: ensure it matches with the open brace (current), then
  // ascend our focus up.  Perform static analysis on the new parent.
  //
  // When parentheses are matched, they will be eliminated from the AST since
  // they represent nothing.  Instead, their first child (operator) will take
  // over the "parent" spot; its children (operands) will remain as the
  // operator's children.
  // current should be the open brace/paren to match against.
  //
  // If the newly-taken-over "parent" spot is at a tree level that starts with
  // an OperatorParser, they should be positions 1 and 0 in the grandparent's
  // children list, respectively, and we will insertNode ourselves into the oP.
  if (!current->parent) {
    throw CompileError("Cannot move above root node " + current->name);
  }

  Brace* open = dynamic_cast<Brace*>(current);
  if (!open) {
    throw CompileError("Found closing brace " + brace->name + " but its parent " + current->name + " is not an open brace");
  }
  if (!open->matchesCloseBrace(brace)) {
    throw CompileError("Incorrect brace/paren match: '" + open->name + "' against '" + n->name + "'");
  }
  Node* parent = current->parent;

  // Before we clamp up the current braces, if our primary child was an
  // OperatorParser, finalize its parse.  It's done.
  if (current->children.size() > 0) { 
    OperatorParser* oP = dynamic_cast<OperatorParser*>(current->children.at(0));
    if (oP) { 
      Node* opTop = oP->finalizeParse();
      current->addChild(opTop);
    } 
  } 

  // Parentheses: these are now useless.  We promote the first child (there
  // must be at least one child!) into the parent (paren) spot; it is the
  // operator, its children are its operands.  Huzzah!
  if (open->isIrrelevant()) {
    // Extract the first child of the open brace; it is the new "operator"
    if (open->children.size() < 1) {
      throw CompileError("Empty parens in the AST are not allowed");
    }
    Node* op = open->children.front();    // "operator" becomes the parent
    open->children.pop_front();
    op->parent = parent;
    // a '({' cannot appear in the input.  Note that actual parentheses in
    // expressions are not given to us as a bare '(' brace.
    if (op->children.size() != 0) {
      throw CompileError("Cannot escalate child " + op->name + " that has " + boost::lexical_cast<string>(op->children.size()) + " > 0 children");
    }
    op->children = open->children;
    open->children.clear();   // Clear open's children so they're not deleted
    // Replace op's children's parent links from open to op
    for (Node::child_iter i = op->children.begin();
         i != op->children.end(); ++i) {
      (*i)->parent = op;
    }
    // Replace parent's child of 'open' with 'op'
    for (Node::child_mod_iter i = parent->children.begin();
         i != parent->children.end(); ++i) {
      if (*i == open) {
        *i = op;
        break;    // a node must only appear once in the AST
      }
    }
    delete open;

    // if the parent's first child is not us, and it's an operatorParser, then
    // we have some cleanup to do.  oP->insertNode(op).  Got that?
    if (2 == parent->children.size()) {
      OperatorParser* oP = dynamic_cast<OperatorParser*>(parent->children.at(0));
      if (oP) {
        parent->children.pop_back();
        oP->insertNode(op);
      }
    }

    // Now that we've moved up into the parent's spot, tell our new primary
    // sibling about us.
    if (parent->children.size() > 1) {
      parent->children.at(0)->initChild(op);
    }

    // Errors from setupAsParent are recoverable
    try {
      op->setupAsParent();
    } catch (CompileError& e) {
      RecoverFromError(e, op);
      throw CompileError(string("Failed to recover from error: ") + e.what());
    }
  } else {
    // Errors from setupAsParent are recoverable
    try {
      open->setupAsParent();
    } catch (CompileError& e) {
      RecoverFromError(e, open);
      throw CompileError(string("Failed to recover from error: ") + e.what());
    }
  }
  delete n;   // always discard the closing brace/paren
  return parent;    // ascend
}

// Find the nearest enclosing block (ancestor) of the given node, and delete
// the subtree from which it came, and (to be paranoid) any of the block's
// children that follow it (there shouldn't be any).
//
// On success, throws a recoveredError with the new "recoveredPosition" that
// should be the AST's new current position (the cleaned-up block).
// On error, throws its own CompileError.
//
// TODO we don't want the nearest enclosing *block*, because block includes
// object and function literals, which we don't want to catch here, I don't
// think.
void Node::RecoverFromError(CompileError& e, Node* problemNode) {
  Node* current = problemNode;
  try {
    while (current && current->parent) {
      Block* parentBlock = dynamic_cast<Block*>(current->parent);
      if (!parentBlock) {
        current = current->parent;
        continue;
      }
      // Find current in the parentBlock's children.  Delete it and any
      // subsequent children.
      parentBlock->removeChildrenStartingAt(current);
      throw RecoveredError(e, parentBlock);
    }
  } catch (CompileError& x) {
    throw CompileError(string("Cannot recover from error '") + e.what() + "': " + x.what());
  }
  if (!current || current->parent || !dynamic_cast<RootNode*>(current)) {
    throw CompileError(string("Cannot recover from error '") + e.what() + "': unknown error");
  }
  // We made it to the root node.
  throw RecoveredError(e, current);
}

/* Members */

Node::Node(Log& log, RootNode*const root, const Token& token)
  : log(log),
    root(root),
    name(token.name),
    value(token.value),
    parent(NULL),
    parentScope(NULL),
    isInit(false),
    isSetup(false),
    isAnalyzed(false),
    isCompiled(false) {
}

Node::~Node() {
  log.debug("Destroying node " + print());
  for (child_rev_iter i = children.rbegin(); i != children.rend(); ++i) {
    //log.debug("    - node " + name + " destroying child " + (*i)->print());
    string childprint = (*i)->print();
    delete *i;
    //log.debug("    - node " + name + " done destroying child " + childprint);
  }
  //log.debug(" - done destroying node " + name);
}

// This is a very early scope initialization.  We don't necessarily have a Node
// parent, and even if we do it may not be trustworthy.  Instead, use the
// provided scopeParent to be something we can use as the parent scope (and if
// it does not itself define the scope, then our parent scope is its parent
// scope).  We even allow nodes to (carefully!) override their own initScope()
// in case they have a real scope as a member that needs to be initialized.
//
// Secondly, if we're initializing a scope, we check if scopeParent's first
// child is 'function' or 'object', in which case the block's scope
// initialization needs to be aware that it's a function or object -- they have
// special scoping rules.
void Node::initScopeNode(Node* scopeParent) {
  if (this == root) {
    throw CompileError("Cannot init scope for the root node");
  }
  parentScope = scopeParent->getScope();
  if (!parentScope) {
    parentScope = scopeParent->getParentScope();
  }
  if (scopeParent->children.size() > 1) {
    Function* function = dynamic_cast<Function*>(scopeParent->children.at(0));
    ObjectLiteral* object = dynamic_cast<ObjectLiteral*>(scopeParent->children.at(0));
    if (function) {
      initScope(parentScope, function);
    } else if (object) {
      initScope(parentScope, object);
    } else {
      initScope(parentScope);
    }
  } else {
    initScope(parentScope);
  }
  isInit = true;
}

// Wipe out all parentScope pointers down the tree; the tree is being
// destroyed, and this notifies Nodes not to try and cleanup (revert) changes
// they've made to their scope.  The cleanup will be done by the enclosing
// scope's owner instead.
void Node::cancelParentScopeNode() {
  cancelParentScope();
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    (*i)->cancelParentScope();
  }
}

void Node::replaceChild(Node* oldChild, Node* newChild) {
  string oldPrint = print();
  bool replaced = false;
  for (child_mod_iter i = children.begin(); i != children.end(); ++i) {
    if (*i == oldChild) {
      *i = newChild;
      replaced = true;
      break;
    }
  }
  if (!replaced) {
    throw CompileError("Failed to replace " + oldChild->print() + " with " + newChild->print() + " in " + print());
  }
  log.debug("Replaced " + oldChild->print() + " in " + oldPrint + " with " + newChild->print() + " to become " + print());
}

// Called only on nodes that are understood to be parents.
// We setupNode() the nodes children-first.
void Node::setupAsParent() {
  // Note: the node's grandchildren should all already be setup.
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    (*i)->setupNode();
  }
  setupNode();
  log.debug("Setup node " + print());
}

void Node::setupNode() {
  if (isSetup) return;
  if (!isInit) {
    throw CompileError("Cannot setup Node " + print() + " until it's init");
  }
  if (!parent) {
    throw CompileError("Cannot setup Node " + print() + " with no parent");
  }
  log.debug(" - setting up node " + print());
  setup();
  isSetup = true;
  log.debug(" - analyzing node " + print());
  analyzeNode();
  isAnalyzed = true;
}

void Node::analyzeNode() {
  if (isAnalyzed) return;
  if (!isInit || !isSetup) {
    throw CompileError("Node " + print() + " cannot do static analysis until init and setup");
  }

  Statement* statement = dynamic_cast<Statement*>(this);
  if (statement) {
    log.debug(" - - analyzing statement " + print());
    statement->analyze();
  }
}

void Node::compileNode() {
  if (isCompiled) {
    throw CompileError("Node " + print() + " has already been compiled");
  }
  if (!isInit || !isSetup || !isAnalyzed) {
    throw CompileError("Node " + print() + " cannot be compiled until init, setup, and analyzed");
  }
  log.debug(" - compiling node " + print());
  compile();
  isCompiled = true;
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

Node::operator string() const {
  return name;
}

/* protected */

void Node::addChild(Node* child) {
  children.push_back(child);
}

void Node::removeChildrenStartingAt(const Node* child) {
  log.debug("Removing children from " + print() + " starting at " + child->print());
  int foundChildren = 0;
  for (child_iter i = children.begin(); i != children.end(); ++i) {
    if (child == *i || foundChildren > 0) {
      ++foundChildren;
      delete *i;
      child = NULL;
    }
  }
  for (int i=0; i < foundChildren; ++i) {
    children.pop_back();
  }
}
