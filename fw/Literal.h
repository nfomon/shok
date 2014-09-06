// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Literal_h_
#define _Literal_h_

#include "FWError.h"
#include "Machine.h"

#include <boost/ptr_container/ptr_vector.hpp>

#include <memory>
#include <utility>
#include <vector>

namespace fw {

class LiteralNode : public MachineNode {
public:
  LiteralNode(Log& log, const std::string& str)
    : MachineNode(log, str),
      m_str(str) {}
  virtual ~LiteralNode() {}

  virtual bool Update(Connector<ListDS>& connector, TreeDS& x, const ListDS* inode) const {
    size_t pos = 0;
    std::string s;
    ListDS* istart = dynamic_cast<ListDS*>(x.istart);
    ListDS* iend = dynamic_cast<ListDS*>(x.iend);
    for (ListDS* i = istart; i != iend; ++i) {
      CharState* state = dynamic_cast<CharState*>(i->GetState());
      s += state.c;
      if (state.c != m_str[pos]) {
        x.ok = false;
      }
      connector.MakeFwdLink(*i, x);
    }
    return true;
  }

  virtual bool Update(Connector<TreeDS>& connector, TreeDS& x, const TreeDS* inode) const {
    throw FWError("Inappropriate TreeDS Update of LiteralNode");
  }

  virtual void UpdateSize(TreeDS& x) const {
    x.size = 0;
    for (TreeDS::child_iter i = x.children.begin(); i != x.children.end(); ++i) {
      x.size += i->size;
    }
  }

private:
  const std::string m_str;
};

struct LiteralState : public MachineState {
  LiteralState(const LiteralNode& node)
    : MachineState(node) {}
  virtual ~LiteralState() {}
};

}

#endif // _Literal_h_
