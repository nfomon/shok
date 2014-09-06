// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Machine_h_
#define _Machine_h_

#include "DS.h"
#include "FWError.h"
#include "State.h"

#include <boost/lexical_cast.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <memory>

namespace fw {

template <typename INode>
class Connector;

class MachineState;

class MachineNode {
public:
  typedef boost::ptr_vector<MachineNode> child_vec;
  typedef child_vec::const_iterator child_iter;

  MachineNode(Log& log, const std::string& name = "")
    : m_log(log),
      m_name(name),
      m_parent(NULL) {}
  virtual ~MachineNode() {}

  virtual std::auto_ptr<State> MakeState() const;

  // Returns true if x requests ownership of the inode
  virtual bool Update(Connector<ListDS>& connector, TreeDS& x, const ListDS* inode) const = 0;
  virtual bool Update(Connector<TreeDS>& connector, TreeDS& x, const TreeDS* inode) const = 0;

  virtual void UpdateSize(TreeDS& x) const = 0;

  void AddChild(std::auto_ptr<MachineNode> child) {
    m_log.info("Adding child " + std::string(*child.get()) + " to " + std::string(*this));
    child->setParent(this);
    m_children.push_back(child);
  }

  child_iter children_begin() const { return m_children.begin(); }
  child_iter children_end() const { return m_children.end(); }

  operator std::string() const { return "(" + m_name + ":" + boost::lexical_cast<std::string>(m_children.size()) + ")"; }

protected:
  void setParent(MachineNode* parent) {
    m_parent = parent;
  }

  Log& m_log;
  std::string m_name;
  MachineNode* m_parent;
  child_vec m_children;
};

struct MachineState : public State {
  const MachineNode& node;
  bool formed;
  MachineState(const MachineNode& node)
    : node(node),
      formed(false) {}
  virtual ~MachineState() {}
};

}

#endif // _Machine_h_
