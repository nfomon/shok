// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Connector.h"
#include "Or.h"

using namespace fw;

bool OrNode::Update(Connector<ListDS>& connector, TreeDS& x, const ListDS* inode) const {
  m_log.info("Updating " + std::string(*this));
  State& xstate = x.GetState();
  OrState* state = dynamic_cast<OrState*>(&xstate);
  if (!state) {
    throw FWError("OrNode " + std::string(*this) + " found inappropriate state");
  }
  // Don't need state... for now.
  for (TreeDS::child_mod_iter i = x.children.begin(); i != x.children.end(); ++i) {
    connector.UpdateNode(*i, inode);
  }

  // Update flags

  return false;
}

bool OrNode::Update(Connector<TreeDS>& connector, TreeDS& x, const TreeDS* inode) const {
  return false;
}

void OrNode::UpdateSize(TreeDS& x) const {
  x.size = 0;
  for (TreeDS::child_iter i = x.children.begin(); i != x.children.end(); ++i) {
    if (i->size > x.size) {
      x.size = i->size;
    }
  }
}

std::auto_ptr<State> OrNode::MakeState() const {
  return std::auto_ptr<State>(new OrState(*this));
}
