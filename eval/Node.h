// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Node_h_
#define _Node_h_

/* AST node
 *
 * Nodes represent the AST and ultimately embody the code to execute.  Each
 * language construct is a subclass of Node.
 *
 * The AST uses the MakeNode factory constructor to create the appropriate
 * subclass for a given AST token.  As the AST gets more tokens it calls
 * InsertNode() which manipulates the node tree and returns the new "current"
 * (insertion) position.
 *
 * Each subclass gets to override initScope(), setup(), and evaluate().  As
 * much static analysis / error checking as possible should happen in setup().
 * evaluate() should be able to assume the Node is in as correct a state as
 * possible and just run the code.  initScope() is a very early initialization
 * of enclosing scopes; the node may not have a real parent or any children
 * yet.
 */

#include "Log.h"
#include "Scope.h"
#include "Token.h"

#include <string>
#include <deque>

namespace eval {

class Block;
class EvalError;
class Function;
class RootNode;

class Node {
public:
  // Construct a Node from a Token
  static Node* MakeNode(Log&, RootNode*const root, const Token&);

  // Insert a Node n into the tree at the provided "current" position.
  // Called by AST; returns the new current position, the original may
  // have been destroyed.
  static Node* InsertNode(Log&, Node* current, Node*);

protected:
  // Called by InsertNode().  When a recoverable error occurs inserting a node,
  // this removes the destructive subtree from the nearest enclosing block, and
  // throws a RecoveredError with the cleaned-up block which the AST will catch
  // and use as the new current position.
  static void RecoverFromError(EvalError& e, Node* problemNode);

  // Creates an operator tree out of the provided node's flattened children.
  // We do this during setupNode(), before we setup() an Expression or TypeSpec.
  static Node* MakeOperatorTree(Node* root);

public:
  virtual ~Node();

  // Caution! Called by rare crazy node-reorganization routines only.
  void replaceChild(Node* oldChild, Node* newChild);
  // Evaluate the node!  Public because it's called by AST on the root node.
  void evaluateNode();
  // Wipe out the node's parentScope; it is being destroyed.
  void cancelParentScopeNode();

  std::string getName() const { return name; }
  std::string getValue() const { return value; }
  bool isNodeEvaluated() const { return isEvaluated; }

  virtual std::string print() const;
  virtual operator std::string() const;

protected:
  friend class OperatorParser;
  typedef std::deque<Node*> child_vec;
  typedef child_vec::const_iterator child_iter;
  typedef child_vec::iterator child_mod_iter;
  typedef child_vec::const_reverse_iterator child_rev_iter;

  Node(Log&, RootNode*const, const Token&);

  // This will be called by InsertNode() only on parent nodes.
  // It calls setupNode() and analyzeNode() on the parent and its children.
  void setupAsParent();
  // Basic, early initialization of a node; used e.g. to initialize parentScope
  void initScopeNode(Node* scopeParent);
  // Validate properties regarding the node's children
  void setupNode();
  void analyzeNode();

  // called by InsertNode()
  void addChild(Node* child);
  // called rather scandalously by RecoverFromError()
  void removeChildrenStartingAt(const Node* child);
  // called by cancelParentScopeNode()
  void cancelParentScope() { parentScope = NULL; }

  virtual void initScope(Scope* scopeParent) {}    // early scope init
  virtual void initScope(Scope* scopeParent, Function* function) {}
  //virtual void initScope(Scope* scopeParent, ObjectLiteral* object) {}
  virtual void initChild(Node* child) {}  // early parent setup for new child
  virtual void setup() = 0;               // child-first setup/analysis
  virtual void evaluate() = 0;            // child-first code execution
  virtual Scope* getScope() { return NULL; }              // local scope
  Scope* getParentScope() const { return parentScope; }   // enclosing scope

  // Set by constructor
  Log& log;
  RootNode*const root;
  std::string name;
  std::string value;
  // Set by InsertNode()
  Node* parent;
  child_vec children;
  // Set by initScope() and cancelParentScope()
  Scope* parentScope;   // nearest enclosing scope (execution context)

  // State flags
  bool isInit;
  bool isSetup;
  bool isAnalyzed;
  bool isEvaluated;
};

}

#endif // _Node_h_
