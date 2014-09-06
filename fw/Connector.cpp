// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Connector.h"
#include "DS.h"

#include <utility>

using namespace fw;

/* public */

template <>
void Connector<ListDS>::Insert(const ListDS& inode) {
  // Left side
  TreeDS* x = NULL;
  if (inode.left) {
    fwdlink_iter fwdlink = m_fwdlinks.find(inode.left);
    if (m_fwdlinks.end() == fwdlink) {
      throw FWError("Inserting inode: Cannot find left link");
    }
    x = fwdlink->second;
  } else {
    x = &m_root;
    x->istart = &inode;
  }

  // Right side
  TreeDS* y = NULL;
  if (inode.right) {
    fwdlink_iter fwdlink = m_fwdlinks.find(inode.right);
    if (m_fwdlinks.end() == fwdlink) {
      throw FWError("Inserting inode: Cannot find right link");
    }
    y = fwdlink->second;
  } else {
    y = &m_root;
  }

  const DS* const old_iend = x->iend;
  Update(*x, &inode);
  // If the left and right sides point to different output-nodes and the left
  // side is unaffected, update the right-side instead.
  if (x != y && old_iend == x->iend) {
    if (m_fwdlinks.find(&inode) != m_fwdlinks.end()) {
      throw FWError("Inserting inode on left side gave it a fwdlink but did not move the left iend");
    }
    Update(*y, &inode);
  }

  if (m_fwdlinks.end() == m_fwdlinks.find(&inode)) {
    throw FWError("Inserting inode failed to associate a fwd link");
  }
}

template <>
void Connector<TreeDS>::Insert(const TreeDS& inode) {
  // TODO
  throw FWError("Connector<TreeDS>::Insert(const TreeDS&) is unimplemented");
}

template <typename INode>
void Connector<INode>::Delete(const INode& inode) {
  fwdlink_iter fwdlink = m_fwdlinks.find(inode);
  if (m_fwdlinks.end() == fwdlink) {
    throw FWError("Deleting inode: Cannot find onode link");
  }
  Update(*fwdlink->second, &inode);
}

template <typename INode>
void Connector<INode>::UpdateNode(TreeDS& x, const INode* inode) {
  // Get state
  State& state = x.GetState();
  MachineState* mstate = dynamic_cast<MachineState*>(&state);
  if (!mstate) {
    throw FWError("UpdateNode expected MachineState");
  }

  // Form children if necessary
  if (!mstate->formed) {
    m_log.info("Forming " + std::string(mstate->node));
    for (MachineNode::child_iter c = mstate->node.children_begin();
         c != mstate->node.children_end(); ++c) {
      x.children.push_back(new TreeDS(c->MakeState(), &x));
    }
  }

  if (mstate->node.Update(*this, x, inode)) {
    MakeFwdLink(*inode, x);
  }
}

template <typename INode>
void Connector<INode>::MakeFwdLink(const INode& inode, TreeDS& x) {
  m_fwdlinks.insert(std::make_pair(&inode, &x));
}

/* private */

template <typename INode>
void Connector<INode>::Update(TreeDS& x, const INode* inode) {
  const DS* old_iend = x.iend;
  UpdateNode(x, inode);
  if (x.parent) {
    if (x.iend != old_iend) {
      Update(*x.parent, dynamic_cast<const INode*>(x.iend));
    } else {
      UpdateSize(x);
    }
  }
}

template <typename INode>
void Connector<INode>::UpdateSize(TreeDS& ix) {
  TreeDS* x = ix;
  while (x) {
    x = x.parent;
  }
}
