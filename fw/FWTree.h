// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _FWTree_h_
#define _FWTree_h_

#include "IConnection.h"
#include "FWError.h"
#include "Hotlist.h"
#include "OutputStrategy.h"
#include "Rule.h"
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
  FWTree* m_parent;

public:
  child_vec children;
  depth_t depth;

  FWTree(const Rule& rule, FWTree* parent, const IList& istart)
    : m_rule(rule),
      m_parent(parent),
      depth(m_parent ? m_parent->depth + 1 : 0),
      m_iconnection(istart),
      m_outputStrategy(rule.MakeOutputStrategy(*this)) {
  }
  virtual ~FWTree() {}

  const State& GetState() const { return m_state; }
  State& GetState() { return m_state; }
  const Rule& GetRule() const { return m_rule; }
  FWTree* GetParent() const { return m_parent; }
  const IList& IStart() const { return m_iconnection.Start(); }
  const IList& IEnd() const { return m_iconnection.End(); }

  IConnection& GetIConnection() { return m_iconnection; }
  OutputStrategy& GetOutputStrategy() const { return *m_outputStrategy.get(); }

  operator std::string() const;
  std::string DrawNode(const std::string& context) const;

  // Clear all state and connection information.  Maintains tree structure.
  void Clear() {
    m_state.Clear();
    m_outputStrategy->Clear();
  }

private:
  IConnection m_iconnection;
  std::auto_ptr<OutputStrategy> m_outputStrategy;
};

}

#endif // _FWTree_h_
