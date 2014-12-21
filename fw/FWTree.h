// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _FWTree_h_
#define _FWTree_h_

#include "Connection.h"
#include "FWError.h"
#include "Hotlist.h"
#include "State.h"

#include "util/Log.h"
#include "util/Graphviz.h"

#include <boost/lexical_cast.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <memory>
#include <vector>

namespace fw {

struct FWTree {
  typedef unsigned int depth_t;
  typedef boost::ptr_vector<FWTree> child_vec;
  typedef child_vec::const_iterator child_iter;
  typedef child_vec::iterator child_mod_iter;

private:
  std::auto_ptr<State> m_state;

public:
  FWTree* parent;
  child_vec children;
  depth_t depth;

  FWTree(Log& log,
         std::auto_ptr<State> state,
         FWTree* parent)
    : m_state(state),
      parent(parent),
      depth(parent ? parent->depth + 1 : 0) {
    if (!m_state.get()) {
      throw FWError("Cannot create FWTree node with NULL state");
    }
    m_oconnection = m_state->rule.MakeOConnection(*this);
  }
  virtual ~FWTree() {}

  State& GetState() const { return *m_state.get(); }
  template <typename StateType>
  StateType& GetState() const {
    StateType* state = dynamic_cast<StateType*>(m_state.get());
    if (!state) {
      throw FWError("Cannot retrieve state to incorrect type");
    }
    return *state;
  }

  OConnection& GetOConnection() const { return *m_oconnection.get(); }
  template <typename OConnType>
  OConnType& GetOConnection() const {
    OConnType* oconn = dynamic_cast<OConnType*>(m_oconnection.get());
    if (!oconn) {
      throw FWError("Cannot retrieve OConnection to incorrect type");
    }
    return *oconn;
  }

  operator std::string() const { return GetState(); }
  std::string print() const {
    std::string s(GetState());
    for (child_iter i = children.begin(); i != children.end(); ++i) {
      s += " (" + i->print() + ")";
    }
    return s;
  }

  std::string DrawNode(const std::string& context) const;

  // Clear all state and connection information.  Maintains tree structure.
  void Clear() {
    State& state = GetState();
    state.Clear();
    iconnection.Clear();
    m_oconnection->Clear();
  }

  IConnection iconnection;  // Connection to the input list

private:
  std::auto_ptr<OConnection> m_oconnection;  // Output list representation
};

struct FWTreeDepthComparator {
  bool operator() (FWTree* a, FWTree* b) const {
    return a->depth < b->depth;
  }
};

struct FWTreeInverseDepthComparator {
  bool operator() (FWTree* a, FWTree* b) const {
    return a->depth > b->depth;
  }
};

}

#endif // _FWTree_h_
