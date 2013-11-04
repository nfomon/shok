// Copyright (C) 2013 Michael Biggs.  See the LICENSE file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _Node_h_
#define _Node_h_

/* AST node
 *
 * Nodes represent the AST and ultimately embody the code to execute.  Each
 * language construct is a subclass of Node.
 *
 * The AST uses the MakeNode factory constructor to create the appropriate
 * subclass for a given AST token.  As the AST gets more tokens it calls
 * insertNode which manipulates the node tree and returns the new "current"
 * (insertion) position.
 *
 * Each subclass gets to override init(), setup(), and evaluate().  As much
 * static analysis / error checking as possible should happen in setup().
 * evaluate() should be able to assume the Node is in as correct a state as
 * possible and just run the code.  init() is only used in special
 * circumstances as a very early member initialization; it is performed
 * top-down the tree but so early that the parent-child relationships aren't
 * fully realized yet.
 */

#include "Log.h"
#include "Scope.h"
#include "Token.h"

#include <string>
#include <deque>

namespace eval {

class Block;
class EvalError;
class RootNode;

class Node {
public:
  static Node* MakeNode(Log&, RootNode*const root, const Token&);

  // Insert a Node n into the tree at the provided "current" position.
  // Called by AST; returns the new current position, the original may
  // have been destroyed.
  static Node* insertNode(Log&, Node* current, Node*);

protected:
  // Called by insertNode().  When a recoverable error occurs inserting a node,
  // this removes the destructive subtree from the nearest enclosing block, and
  // throws a RecoveredError with the cleaned-up block which the AST will catch
  // and use as the new current position.
  static void recoverFromError(EvalError& e, Node* problemNode);

public:
  virtual ~Node();

  // Caution! Called by rare crazy node-reorganization routines only.
  void replaceChild(Node* oldChild, Node* newChild);
  // Evaluate the node!  Public because it's called by AST on the root node.
  void evaluateNode();

  std::string getName() const { return name; }
  std::string getValue() const { return value; }
  bool isNodeEvaluated() const { return isEvaluated; }

  virtual std::string print() const;
  virtual operator std::string() const;

protected:
  friend class Expression;
  Node(Log&, RootNode*const, const Token&);

  typedef std::deque<Node*> child_vec;
  typedef child_vec::const_iterator child_iter;
  typedef child_vec::iterator child_mod_iter;

  // This will be called by insertNode() only on parent nodes.
  // It calls setupNode() and analyzeNode() on the parent and its children.
  void setupAsParent();
  // Basic, early initialization of a node; used e.g. to initialize parentScope
  void initNode();
  // Validate properties regarding the node's children
  void setupNode();
  void analyzeNode();

  // called by insertNode()
  void addChild(Node* child);
  // called rather scandalously by recoverFromError()
  void removeChildrenStartingAt(const Node* child);

  virtual void init() {}                // early parent-first init pass
  virtual void setup() = 0;             // child-first setup/analysis
  virtual void evaluate() = 0;          // child-first code execution
  virtual void cleanup(bool error) {}   // child-first cleanup
  virtual Scope* getScope() { return NULL; }              // local scope
  Scope* getParentScope() const { return parentScope; }   // enclosing scope
  void setParentScope(Scope* scope) { parentScope = scope; }

  // State flags
  bool isInit;
  bool isSetup;
  bool isAnalyzed;
  bool isEvaluated;

  // Set by constructor
  Log& log;
  RootNode*const root;
  std::string name;
  std::string value;
  // Set by insertNode()
  Node* parent;
  child_vec children;
  // Set by init()
  Scope* parentScope;   // nearest enclosing scope (execution context)
};

};

#endif // _Node_h_
