// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _OrderList_h_
#define _OrderList_h_

#include "IList.h"

#include <map>
#include <string>
#include <vector>

namespace statik {

class OrderNode {
public:
  typedef std::map<const IList*, OrderNode*> node_map;
  typedef node_map::const_iterator node_iter;
  typedef node_map::iterator node_mod_iter;

  OrderNode(OrderNode* parent);
  ~OrderNode();

  void RemoveLeaf(const IList& leaf);
  void InsertLeaf(const IList& leaf, node_map& out_changes);
  void InsertLeafBefore(const IList& leaf, const IList& next, node_map& out_changes);
  void InsertLeafAfter(const IList& leaf, const IList& prev, node_map& out_changes);
  void InsertChildAfter(OrderNode* child, const OrderNode& prev, node_map& out_changes);

  int CompareLeaves(const IList& a, const IList& b) const;
  int CompareChildren(const OrderNode& a, const OrderNode& b) const;

  bool HasLeaves() const;
  const OrderNode* Parent() const;

  std::string DrawNode(const std::string& context) const;

private:
  OrderNode* m_parent;
  const unsigned int MAX_CHILDREN;
  const unsigned int MAX_LEAVES;

  typedef std::vector<OrderNode*> child_vec;
  typedef child_vec::const_iterator child_iter;
  typedef child_vec::iterator child_mod_iter;
  child_vec m_children;
  typedef std::vector<const IList*> leaf_vec;
  typedef leaf_vec::const_iterator leaf_iter;
  typedef leaf_vec::iterator leaf_mod_iter;
  leaf_vec m_leaves;

  bool HasChild(const OrderNode& child) const;
  void RemoveChild(const OrderNode& child);
  void Grow(node_map& out_changes);
};

class OrderList {
public:
  ~OrderList();
  void Insert(const IList& inode);
  void Delete(const IList& inode);
  int Compare(const IList& a, const IList& b) const;

  std::string Draw(const std::string& context, const IList& start) const;

private:
  OrderNode::node_map m_nodes;
};

}

#endif // _OrderList_h_
