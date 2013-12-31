// Copyright (C) 2013 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _OperatorParser_h_
#define _OperatorParser_h_

/* Operator Parser
 *
 * An OperatorParser is a Node that is provided by the input-AST alongside a
 * flattened operator tree.  For example:
 *
 *    (exp (var ID:'a') PLUS (var ID:'b') STAR MINUS (var ID:'c'))
 *
 * Expression and TypeSpec are two OperatorParsers.
 *
 * The OperatorParser is noticed by Node::InsertNode() and handles the logic
 * for parsing these special structures; organizing the operators on-the-fly
 * as-they-come into a tree that accounts for operator precedence while
 * simultaneously allowing us to perform early static analysis on any of the
 * elements between operators.
 *
 * Specifically, we employ Pratt (aka TDOP: Top-Down Operator Precedence
 * parsing) to let us setup() nodes as quickly as possible.
 */

#include "Log.h"
#include "Node.h"
#include "Operator.h"

#include <string>
#include <utility>
#include <vector>

namespace eval {

class OperatorParser {
public:
  OperatorParser(Log& log)
    : m_log(log),
      m_infixing(false) {}
  void insertNode(Node* node);
  Node* finalizeParse();

private:
  Log& m_log;
  bool m_infixing;
  typedef std::pair<Node*,Operator::op_priority> stack_pair;
  typedef std::vector<stack_pair> stack_vec;
  stack_vec m_stack;
};

};

#endif // _OperatorParser_h_
