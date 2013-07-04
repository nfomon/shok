#ifndef _Node_h_
#define _Node_h_

/* AST node
 *
 * Use the MakeNode factory constructor to create the appropriate
 * subclass for a given AST token.  At construction, not all of its
 * members will be set to their correct values.  First you must add
 * all its child nodes via addChild(), then call complete().  This
 * will run some validation checks on the node, complete() its
 * children, and mark it as ready for use.
 */

#include "Log.h"
#include "Token.h"

#include <string>
#include <deque>

namespace eval {

class Block;

class Node {
public:
  friend class AST;

  static Node* MakeNode(Log&, const Token&);

  Node(Log&, const Token&);
  virtual ~Node();

  void addChild(Node* child);
  void setup();
  void setupAndCompleteAsParent();
  // Reorder operator/expression trees for correct operator precedence
  // Not intended to be overridden by anything other than RootNode
  virtual void reorderOperators();
  // Not intended to be overridden by anything other than RootNode
  virtual void staticAnalysis();
  bool isSetup() { return m_isSetup; }
  bool isComplete() { return m_isComplete; }
  bool isReordered() { return m_isReordered; }
  bool isAnalyzed() { return m_isAnalyzed; }

  virtual std::string print() const;
  virtual void complete() = 0;    // make protected?
  virtual void analyze() { m_isAnalyzed = true; }
  virtual void evaluate() = 0;
  virtual operator std::string() const;

protected:
  typedef std::deque<Node*> child_vec;
  typedef child_vec::const_iterator child_iter;
  typedef child_vec::iterator child_mod_iter;

  // Set by constructor
  Log& log;
  std::string name;
  std::string value;
  bool m_isSetup;
  bool m_isComplete;
  bool m_isReordered;
  bool m_isAnalyzed;
  // Set by AST prior to completion
  Node* parent;
  child_vec children;
  // Set by setup(), parent-first
  Block* block;   // the most recent ancestor block (execution context)
};

};

#endif // _Node_h_
