// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _FWTree_h_
#define _FWTree_h_

#include "Connection.h"
#include "FWError.h"
#include "Hotlist.h"
#include "State.h"

#include "util/Graphviz.h"

#include <boost/lexical_cast.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <memory>
#include <vector>

namespace fw {

class FWTree {
public:
  typedef unsigned int depth_t;
  typedef boost::ptr_vector<FWTree> child_vec;
  typedef child_vec::const_iterator child_iter;
  typedef child_vec::iterator child_mod_iter;

private:
  const Rule& m_rule;
  State m_state;

public:
  FWTree* parent;
  child_vec children;
  depth_t depth;

  FWTree(const Rule& rule, FWTree* parent)
    : m_rule(rule),
      parent(parent),
      depth(parent ? parent->depth + 1 : 0) {
    m_oconnection = rule.MakeOConnection(*this);
  }
  virtual ~FWTree() {}

  const State& GetState() const { return m_state; }
  State& GetState() { return m_state; }
  const Rule& GetRule() const { return m_rule; }

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
    m_state.Clear();
    iconnection.Clear();
    m_oconnection->Clear();
  }

  IConnection iconnection;  // Connection to the input list

private:
  std::auto_ptr<OConnection> m_oconnection;  // Output list representation
};

}

#endif // _FWTree_h_
