// Copyright (C) 2015 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "OrderList.h"

#include "SError.h"
#include "SLog.h"

#include "util/Graphviz.h"
using Util::dotVar;

#include <memory>
#include <set>
#include <string>
#include <vector>
using std::auto_ptr;
using std::set;
using std::string;
using std::vector;

using namespace statik;

/* OrderNode public */

OrderNode::OrderNode(OrderNode* parent)
  : m_parent(parent),
    MAX_CHILDREN(5),
    MAX_LEAVES(5) {
}

OrderNode::~OrderNode() {
  try {
    if (m_parent && m_parent->HasChild(*this)) {
      m_parent->RemoveChild(*this);
    }
    if (m_parent && m_parent->m_children.empty()) {
      delete m_parent;
      m_parent = NULL;
    }
  } catch (const SError& e) {
    g_log.error() << "Problem destructing OrderNode: " << e.what();
  } catch (...) {
    g_log.error() << "Unknown problem destructing OrderNode";
  }
}

bool OrderNode::HasChild(const OrderNode& child) const {
  for (child_iter i = m_children.begin(); i != m_children.end(); ++i) {
    if (*i == &child) {
      return true;
    }
  }
  return false;
}

void OrderNode::RemoveChild(const OrderNode& child) {
  //g_log.debug() << "OrderNode: removing child";
  for (child_mod_iter i = m_children.begin(); i != m_children.end(); ++i) {
    if (*i == &child) {
      m_children.erase(i);
      if (m_parent && m_children.empty()) {
        m_parent->RemoveChild(*this);
        if (m_parent->m_children.empty()) {
          delete m_parent;
          m_parent = NULL;
        }
      }
      return;
    }
  }
  throw SError("Failed to remove child from OrderNode");
}

void OrderNode::RemoveLeaf(const List& leaf) {
  //g_log.debug() << "OrderNode: removing leaf " << leaf;
  for (leaf_mod_iter i = m_leaves.begin(); i != m_leaves.end(); ++i) {
    if (*i == &leaf) {
      m_leaves.erase(i);
      if (m_parent && m_leaves.empty()) {
        m_parent->RemoveChild(*this);
        if (m_parent->m_children.empty()) {
          delete m_parent;
          m_parent = NULL;
        }
      }
      return;
    }
  }
  throw SError("Failed to remove leaf from OrderNode");
}

void OrderNode::InsertLeaf(const List& leaf, node_map& out_changes) {
  //g_log.debug() << "OrderNode: Inserting leaf " << leaf;
  m_leaves.push_back(&leaf);
  out_changes[&leaf] = this;
  if (m_leaves.size() > MAX_LEAVES) {
    Grow(out_changes);
  }
}

void OrderNode::InsertLeafBefore(const List& leaf, const List& next, node_map& out_changes) {
  //g_log.debug() << "OrderNode: Inserting leaf " << leaf << " before " << next;
  for (leaf_mod_iter i = m_leaves.begin(); i != m_leaves.end(); ++i) {
    if (*i == &next) {
      m_leaves.insert(i, &leaf);
      out_changes[&leaf] = this;
      if (m_leaves.size() > MAX_LEAVES) {
        Grow(out_changes);
      }
      return;
    }
  }
  throw SError("Failed to find next-leaf " + string(next) + " to insert " + string(leaf) + " before");
}

void OrderNode::InsertLeafAfter(const List& leaf, const List& prev, node_map& out_changes) {
  //g_log.debug() << "OrderNode: Inserting leaf " << leaf << " after " << prev;
  for (leaf_mod_iter i = m_leaves.begin(); i != m_leaves.end(); ++i) {
    if (*i == &prev) {
      ++i;
      m_leaves.insert(i, &leaf);
      out_changes[&leaf] = this;
      if (m_leaves.size() > MAX_LEAVES) {
        Grow(out_changes);
      }
      return;
    }
  }
  throw SError("Failed to find prev-leaf " + string(prev) + " to insert " + string(leaf) + " after");
}

void OrderNode::InsertChildAfter(OrderNode* child, const OrderNode& prev, node_map& out_changes) {
  //g_log.debug() << "OrderNode: Inserting child after another";
  for (child_mod_iter i = m_children.begin(); i != m_children.end(); ++i) {
    if (*i == &prev) {
      ++i;
      m_children.insert(i, child);
      if (m_children.size() > MAX_CHILDREN) {
        Grow(out_changes);
      }
      return;
    }
  }
  throw SError("Failed to find prev-child to insert another child after");
}

int OrderNode::CompareLeaves(const List& a, const List& b) const {
  g_log.debug() << "OrderNode: comparing leaves " << a << " and " << b;
  for (leaf_iter i = m_leaves.begin(); i != m_leaves.end(); ++i) {
    if (&a == *i && &b == *i) {
      return 0;
    } else if (&a == *i) {
      return 1;
    } else if (&b == *i) {
      return -1;
    }
  }
  throw SError("Failed to compare leaves of OrderNode");
}

int OrderNode::CompareChildren(const OrderNode& a, const OrderNode& b) const {
  //g_log.debug() << "OrderNode: comparing children";
  for (child_iter i = m_children.begin(); i != m_children.end(); ++i) {
    if (&a == *i && &b == *i) {
      return 0;
    } else if (&a == *i) {
      return 1;
    } else if (&b == *i) {
      return -1;
    }
  }
  throw SError("Failed to compare children of OrderNode");
}

bool OrderNode::HasLeaves() const {
  return !m_leaves.empty();
}

const OrderNode* OrderNode::Parent() const {
  return m_parent;
}

string OrderNode::DrawNode(const string& context) const {
  string s = dotVar(this, context) + " [style=\"filled\", fillcolor=\"#c0d0f0\"];\n";
  if (m_parent) {
    s += dotVar(this, context) + " -> " + dotVar(m_parent, context) + " [constraint=false, weight=0, style=\"dashed\", arrowsize=0.5, color=\"#3060f0\"];\n";
  }
  if (!m_leaves.empty()) {
    s += "{ rank=same;\n";
    leaf_iter prev_i = m_leaves.begin();
    for (leaf_iter i = m_leaves.begin() + 1; i != m_leaves.end(); ++i) {
      s += dotVar(*prev_i, context) + " -> " + dotVar(*i, context) + " [style=\"invis\"];\n";
      prev_i = i;
    }
    s += "}\n";
    for (leaf_iter i = m_leaves.begin(); i != m_leaves.end(); ++i) {
      s += dotVar(this, context) + " -> " + dotVar(*i, context) + ";\n";
    }
  }
  if (!m_children.empty()) {
    s += "{ rank=same;\n";
    child_iter prev_i = m_children.begin();
    for (child_iter i = m_children.begin() + 1; i != m_children.end(); ++i) {
      s += dotVar(*prev_i, context) + " -> " + dotVar(*i, context) + " [style=\"invis\"];\n";
      prev_i = i;
    }
    s += "}\n";
    for (child_iter i = m_children.begin(); i != m_children.end(); ++i) {
      s += dotVar(this, context) + " -> " + dotVar(*i, context) + ";\n";
    }
  }
  return s;
}

/* OrderNode private */

void OrderNode::Grow(node_map& out_changes) {
  if (MAX_LEAVES+1 == m_leaves.size()) {
    if (!m_parent) {
      // New parent is the root
      m_parent = new OrderNode(NULL);
      m_parent->m_children.push_back(this);
    }
    auto_ptr<OrderNode> right(new OrderNode(m_parent));
    right->m_leaves.push_back(m_leaves.back());
    out_changes[m_leaves.back()] = right.get();
    m_leaves.pop_back();
    m_parent->InsertChildAfter(right.release(), *this, out_changes);
  } else if (MAX_CHILDREN+1 == m_children.size()) {
    if (!m_parent) {
      // New parent is the root
      m_parent = new OrderNode(NULL);
      m_parent->m_children.push_back(this);
    }
    auto_ptr<OrderNode> right(new OrderNode(m_parent));
    right->m_children.push_back(m_children.back());
    m_children.back()->m_parent = right.get();
    m_children.pop_back();
    m_parent->InsertChildAfter(right.release(), *this, out_changes);
  } else {
    throw SError("OrderNode growing at wrong time");
  }
}

/* OrderList */

OrderList::~OrderList() {
  g_log.debug() << "Destructing OrderList";
  for (OrderNode::node_mod_iter i = m_nodes.begin(); i != m_nodes.end(); ++i) {
    i->second->RemoveLeaf(*i->first);
    if (!i->second->HasLeaves()) {
      delete i->second;
    }
  }
}

void OrderList::Insert(const List& inode) {
  g_log.debug() << "OrderList: inserting " << inode;
  OrderNode::node_map changes;
  {
    List* left = inode.left;
    while (left && m_nodes.end() == m_nodes.find(left)) {
      left = left->left;
    }
    if (left) {
      OrderNode::node_iter i = m_nodes.find(left);
      if (i != m_nodes.end()) {
        i->second->InsertLeafAfter(inode, *left, changes);
        for (OrderNode::node_iter i = changes.begin(); i != changes.end(); ++i) {
          m_nodes[i->first] = i->second;
        }
        return;
      }
    }
  } {
    List* right = inode.right;
    while (right && m_nodes.end() == m_nodes.find(right)) {
      right = right->right;
    }
    if (right) {
      OrderNode::node_iter i = m_nodes.find(right);
      if (i != m_nodes.end()) {
        i->second->InsertLeafBefore(inode, *right, changes);
        for (OrderNode::node_iter i = changes.begin(); i != changes.end(); ++i) {
          m_nodes[i->first] = i->second;
        }
        return;
      }
    }
  }
  // New tree
  OrderNode* root(new OrderNode(/*parent*/ NULL));
  root->InsertLeaf(inode, changes);
  for (OrderNode::node_iter i = changes.begin(); i != changes.end(); ++i) {
    m_nodes[i->first] = i->second;
  }
}

void OrderList::Delete(const List& inode) {
  g_log.debug() << "OrderList: deleting " << inode;
  OrderNode::node_iter i = m_nodes.find(&inode);
  if (m_nodes.end() == i) {
    throw SError("Cannot delete node from OrderList; node not found");
  }
  i->second->RemoveLeaf(inode);
  if (!i->second->HasLeaves()) {
    delete i->second;
  }
  m_nodes.erase(&inode);
}

int OrderList::Compare(const List& a, const List& b) const {
  g_log.info() << "OrderList: comparing " << a << " and " << b;
  OrderNode::node_iter ai = m_nodes.find(&a);
  OrderNode::node_iter bi = m_nodes.find(&b);
  if (m_nodes.end() == ai) {
    throw SError("Failed to find first node for OrderList::Compare()");
  } else if (m_nodes.end() == bi) {
    throw SError("Failed to find second node for OrderList::Compare()");
  }
  if (ai->second == bi->second) {
    return ai->second->CompareLeaves(a, b);
  }
  const OrderNode* an = ai->second;
  const OrderNode* bn = bi->second;
  const OrderNode* an_prev = an;
  const OrderNode* bn_prev = bn;
  while (an != bn) {
    an_prev = an;
    bn_prev = bn;
    an = an->Parent();
    bn = bn->Parent();
    if (!an || !bn) {
      throw SError("OrderList comparison failed; nodes are in different trees");
    }
  }
  return an->CompareChildren(*an_prev, *bn_prev);
}

string OrderList::Draw(const string& context, const List& start) const {
  typedef vector<const OrderNode*> level_vec;
  typedef level_vec::const_iterator level_iter;
  level_vec level;
  set<const OrderNode*> levelset;
  for (const List* i = &start; i != NULL; i = i->right) {
    OrderNode::node_iter node = m_nodes.find(i);
    if (node != m_nodes.end() && levelset.end() == levelset.find(node->second)) {
      level.push_back(node->second);
      levelset.insert(node->second);
    }
  }
  string s = "/*list*/\n{ rank=same;\n";
  s += start.DrawNode(context);   // draws the whole List
  s += "}\n";
  while (!level.empty()) {
    level_vec nextLevel;
    s += "\n/*level:*/\n";
    for (level_iter i = level.begin(); i != level.end(); ++i) {
      s += "\n/* - node:*/\n";
      s += (*i)->DrawNode(context);
      if ((*i)->Parent() && (nextLevel.empty() || nextLevel.back() != (*i)->Parent())) {
        nextLevel.push_back((*i)->Parent());
      }
    }
    level = nextLevel;
  }
  return s;
}
