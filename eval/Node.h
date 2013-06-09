#ifndef _Node_h_
#define _Node_h_

/* AST node */

#include "Token.h"

#include <string>
#include <deque>

namespace eval {

struct Node {
  Node(const Token& token);
  virtual ~Node();

  std::string print() const;
  void addChild(Node* child);
  void evaluate();

  bool completed;
  std::string name;
  std::string value;
  Node* parent;
  typedef std::deque<Node*> child_vec;
  typedef std::deque<Node*>::const_iterator child_vec_iter;
  child_vec children;
};

};

#endif // _Node_h_
