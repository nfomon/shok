// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Connector_h_
#define _Connector_h_

#include "Machine.h"
#include "DS.h"

#include "util/Log.h"

#include <map>

namespace fw {

template <typename INode>
class Connector {
public:
  Connector(Log& log, const MachineNode& machine)
    : m_log(log),
      m_root(machine.MakeState()) {}

  // Call Insert() or Delete() AFTER you update the inode's connections in the
  // input DS, but before deleting the node.  Connector will update its links.
  void Insert(const INode& inode);
  void Delete(const INode& inode);

  // Updates a node, forming and/or updating its children as needed, then
  // determining the node's state.  Does not move "upwards" after that.  Called
  // by Update() and also by the MachineNode::Update()s for the children.
  void UpdateNode(TreeDS& x, const INode* inode);

private:
  typedef std::map<const INode*, TreeDS*> fwdlink_map;
  typedef typename fwdlink_map::const_iterator fwdlink_iter;

  // Update a node (via UpdateNode) and move "up" the output tree if it changes
  // its bounds.
  void Update(TreeDS& x, const INode* inode);

  // Set the fwd link from the inode to this output node
  void MakeFwdLink(const INode& inode, TreeDS& x);

  Log& m_log;
  TreeDS m_root;
  fwdlink_map m_fwdlinks;
};

template <>
void Connector<ListDS>::Insert(const ListDS& inode);

template <>
void Connector<TreeDS>::Insert(const TreeDS& inode);

}

#endif // _Connector_h_
