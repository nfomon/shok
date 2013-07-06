// Copyright (C) 2013 Michael Biggs.  See the COPYRIGHT file at the top-level
// directory of this distribution and at http://lush-shell.org/copyright.html

#ifndef _Node_h_
#define _Node_h_

/* AST node
 *
 * Use the MakeNode factory constructor to create the appropriate
 * subclass for a given AST token.  At construction, not all of its
 * members will be set to their correct values.  First you must add
 * all its child nodes via addChild(), then call setup().  This
 * will run some validation checks on the node, setup() its
 * children, and mark it as ready for use.
 */

#include "Log.h"
#include "Token.h"

#include <string>
#include <deque>

namespace eval {

class Block;
class RootNode;

class Node {
public:
  static Node* MakeNode(Log&, RootNode*const root, const Token&);

  // Insert a Node n into the tree at the provided "current" position.
  // Called by AST; returns the new current position, the original may
  // have been destroyed.
  static Node* insertNode(Node* current, Node*);

  virtual ~Node();

  // This will be called by insertNode() only on parent nodes.
  // It should be the only thing that calls setupNode().
  void setupAsParent();
  // Validate properties regarding the node's children
  void setupNode();
  // Reorder operator/expression trees for correct operator precedence
  void reorderOperators();
  // Not intended to be overridden by anything other than RootNode
  void analyzeNode();
  void evaluateNode();
  bool isNodeEvaluated() { return isEvaluated; }

  virtual std::string print() const;
  virtual operator std::string() const;

protected:
  Node(Log&, RootNode*const, const Token&);

  typedef std::deque<Node*> child_vec;
  typedef child_vec::const_iterator child_iter;
  typedef child_vec::iterator child_mod_iter;

  void addChild(Node* child);
  virtual void setup() = 0;
  virtual void analyzeDown() {}   // Parent-first static analysis
  virtual void analyzeUp() {}     // Child-first static analysis
  virtual void evaluate() = 0;

  // State flags
  bool isSetup;
  bool isReordered;
  bool isAnalyzed;
  bool isEvaluated;

  // Set by constructor
  Log& log;
  RootNode*const root;
  std::string name;
  std::string value;
  // Set by addToken()
  Node* parent;
  child_vec children;
  // Set by analyzeNode(), parent-first
  Block* parentBlock;   // nearest enclosing block (execution context)
  // Set by analyzeUp()
  //Type type;
};

};

#endif // _Node_h_
