// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _DS_h_
#define _DS_h_

#include "State.h"
#include "FWError.h"

#include <boost/ptr_container/ptr_vector.hpp>

#include <memory>

namespace fw {

struct DS {
  DS(std::auto_ptr<State> state)
    : m_state(state),
      istart(NULL),
      iend(NULL),
      size(0) {
    if (!m_state.get()) {
      throw FWError("Cannot create DS node with NULL state");
    }
  }
  virtual ~DS() {}

  State& GetState() const { return *m_state.get(); }
  const DS* istart;
  const DS* iend;
  size_t size;

private:
  std::auto_ptr<State> m_state;
};

struct ListDS : public DS {
  ListDS(std::auto_ptr<State> state,
         ListDS* left = NULL,
         ListDS* right = NULL)
    : DS(state),
      left(left),
      right(right),
      size(1) {}
  virtual ~ListDS() {}

  ListDS* left;
  ListDS* right;
};

struct TreeDS : public DS {
  typedef boost::ptr_vector<TreeDS> child_vec;
  typedef child_vec::const_iterator child_iter;
  typedef child_vec::iterator child_mod_iter;

  TreeDS* parent;
  child_vec children;

  TreeDS(std::auto_ptr<State> state,
         TreeDS* parent = NULL)
    : DS(state),
      parent(parent) {}
  virtual ~TreeDS() {}
};

}

#endif // _DS_h_
