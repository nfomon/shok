// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Or_h_
#define _Or_h_

#include "Machine.h"

#include <boost/ptr_container/ptr_vector.hpp>

#include <memory>
#include <utility>
#include <vector>

namespace fw {

struct OrNode : public MachineNode {
  OrNode(Log& log, const std::string& name = "")
    : MachineNode(log, name) {}
  virtual ~OrNode() {}
  virtual bool Update(Connector<ListDS>& connector, TreeDS& x, const ListDS* inode) const;
  virtual bool Update(Connector<TreeDS>& connector, TreeDS& x, const TreeDS* inode) const;
  virtual void UpdateSize(TreeDS& x) const;
  virtual std::auto_ptr<State> MakeState() const;
};

struct OrState : public MachineState {
public:
  OrState(const OrNode& node)
    : MachineState(node) {}
  virtual ~OrState() {}
};

}

#endif // _Or_h_
