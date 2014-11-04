// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _DS_h_
#define _DS_h_

#include "Hotlist.h"
#include "State.h"
#include "FWError.h"

#include <boost/ptr_container/ptr_vector.hpp>

#include <memory>
#include <vector>

namespace fw {

struct DS {
  DS(std::auto_ptr<State> state)
    : istart(NULL),
      iend(NULL),
      size(0),
      m_state(state) {
    if (!m_state.get()) {
      throw FWError("Cannot create DS node with NULL state");
    }
  }
  virtual ~DS() {}

  virtual operator std::string() const = 0;
  virtual std::string print() const = 0;

  State& GetState() const { return *m_state.get(); }
  template <typename StateType>
  StateType& GetState() const {
    StateType* state = dynamic_cast<StateType*>(m_state.get());
    if (!state) {
      throw FWError("Cannot retrieve state to incorrect type");
    }
    return *state;
  }
  const DS* istart; // starting inode
  const DS* iend;   // inode that makes us bad after we were done, or last inode
  size_t size;      // number of inodes that make us good (count: iend - istart)
  Hotlist hotlist;

private:
  std::auto_ptr<State> m_state;
};

struct ListDS : public DS {
  ListDS(std::auto_ptr<State> state,
         ListDS* left = NULL,
         ListDS* right = NULL)
    : DS(state),
      left(left),
      right(right) {}
  virtual ~ListDS() {}

  virtual operator std::string() const { return "(ListDS " + std::string(GetState()) + ")"; }
  virtual std::string print() const {
    std::string s = "<" + std::string(GetState()) + ">";
    if (right) {
      s += "-" + right->print();
    }
    return s;
  }

  ListDS* left;
  ListDS* right;
};

struct TreeDS : public DS {
  typedef unsigned int depth_t;
  typedef boost::ptr_vector<TreeDS> child_vec;
  typedef child_vec::const_iterator child_iter;
  typedef child_vec::iterator child_mod_iter;

  TreeDS* parent;
  child_vec children;
  depth_t depth;

  TreeDS(std::auto_ptr<State> state,
         TreeDS* parent)
    : DS(state),
      parent(parent),
      depth(parent ? parent->depth + 1 : 0) {}
  virtual ~TreeDS() {}

  virtual operator std::string() const { return "(TreeDS " + std::string(GetState()) + ")"; }
  virtual std::string print() const {
    std::string s(GetState());
    for (child_iter i = children.begin(); i != children.end(); ++i) {
      s += " (" + i->print() + ")";
    }
    return s;
  }

  void Clear() {
    State& state = GetState();
    state.Clear();
    istart = NULL;
    iend = NULL;
    size = 0;
  }
};

struct TreeDSDepthComparator {
  bool operator() (TreeDS* a, TreeDS* b) const {
    return a->depth < b->depth;
  }
};

struct TreeDSInverseDepthComparator {
  bool operator() (TreeDS* a, TreeDS* b) const {
    return a->depth > b->depth;
  }
};

}

#endif // _DS_h_
