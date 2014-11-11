// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _DS_h_
#define _DS_h_

#include "FWError.h"
#include "Hotlist.h"
#include "OData.h"
#include "State.h"

#include <boost/ptr_container/ptr_vector.hpp>

#include <memory>
#include <vector>

namespace fw {

struct IList;

struct IConnection {
  IConnection()
    : istart(NULL),
      iend(NULL),
      size(0) {}
  const IList* istart;  // starting inode
  const IList* iend;    // inode that makes us bad after we're done; NULL => eoi
  size_t size;    // number of inodes that make us good (count: iend - istart)

  void Clear() {
    istart = NULL;
    iend = NULL;
    size = 0;
  }
};

struct OConnection {
  OConnection()
    : ostart(NULL),
      oend(NULL),
      size(0) {}
  // If leaf node, this is the output list node for the leaf.
  std::auto_ptr<IList> oleaf;
  IList* ostart;  // Non-leaf node: first oleaf node that is spanned
  IList* oend;    // Non-leaf node: last oleaf node that is spanned
  size_t size;    // number of oleaf nodes that are spanned
  Hotlist hotlist;  // oleaf nodes that have been added/removed from the list

  void Clear() {
    oleaf.reset();
    ostart = NULL;
    oend = NULL;
    size = 0;
    hotlist.clear();
  }
};

struct IList {
  IList(std::auto_ptr<OData> data,
        IList* left = NULL,
        IList* right = NULL)
    : m_data(data),
      left(left),
      right(right) {
  }

private:
  std::auto_ptr<OData> m_data;

public:
  OData& GetData() const { return *m_data.get(); }
  template <typename DataType>
  DataType& GetData() const {
    DataType* data = dynamic_cast<DataType*>(m_data.get());
    if (!data) {
      throw FWError("Cannot retrieve data to incorrect type");
    }
    return *data;
  }

  operator std::string() const { return "(IList " + std::string(GetData()) + ")"; }
  std::string print() const {
    std::string s = "<" + std::string(GetData()) + ">";
    if (right) {
      s += "-" + right->print();
    }
    return s;
  }

  IList* left;
  IList* right;
};

struct TreeDS {
  typedef unsigned int depth_t;
  typedef boost::ptr_vector<TreeDS> child_vec;
  typedef child_vec::const_iterator child_iter;
  typedef child_vec::iterator child_mod_iter;

private:
  std::auto_ptr<State> m_state;

public:
  TreeDS* parent;
  child_vec children;
  depth_t depth;

  TreeDS(std::auto_ptr<State> state,
         TreeDS* parent)
    : m_state(state),
      parent(parent),
      depth(parent ? parent->depth + 1 : 0) {
    if (!m_state.get()) {
      throw FWError("Cannot create TreeDS node with NULL state");
    }
  }
  virtual ~TreeDS() {}

  State& GetState() const { return *m_state.get(); }
  template <typename StateType>
  StateType& GetState() const {
    StateType* state = dynamic_cast<StateType*>(m_state.get());
    if (!state) {
      throw FWError("Cannot retrieve state to incorrect type");
    }
    return *state;
  }

  operator std::string() const { return "(TreeDS " + std::string(GetState()) + ")"; }
  std::string print() const {
    std::string s(GetState());
    for (child_iter i = children.begin(); i != children.end(); ++i) {
      s += " (" + i->print() + ")";
    }
    return s;
  }

  // Clear all state and connection information.  Maintains tree structure.
  void Clear() {
    State& state = GetState();
    state.Clear();
    iconnection.Clear();
    oconnection.Clear();
  }

  IConnection iconnection;  // Connection to the input list
  OConnection oconnection;  // Output list representation
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
