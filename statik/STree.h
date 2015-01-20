// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _STree_h_
#define _STree_h_

#include "ComputeFunc.h"
#include "Connector.h"
#include "IConnection.h"
#include "SError.h"
#include "Hotlist.h"
#include "OutputFunc.h"
#include "RestartFunc.h"
#include "Rule.h"
#include "State.h"

#include "util/Graphviz.h"

#include <boost/lexical_cast.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <memory>
#include <ostream>
#include <vector>

namespace statik {

class STree {
public:
  typedef unsigned int depth_t;
  typedef boost::ptr_vector<STree> child_vec;
  typedef child_vec::const_iterator child_iter;
  typedef child_vec::iterator child_mod_iter;

private:
  Connector& m_connector;
  const Rule& m_rule;
  State m_state;
  STree* m_parent;

public:
  child_vec children;
  depth_t depth;

  STree(Connector& connector,
         const Rule& rule, STree* parent,
         std::auto_ptr<RestartFunc> restartFunc,
         std::auto_ptr<ComputeFunc> computeFunc,
         std::auto_ptr<OutputFunc> outputFunc);

  void RestartNode(const IList& istart);
  bool ComputeNode();
  void ClearNode();

  const State& GetState() const { return m_state; }
  State& GetState() { return m_state; }
  Connector& GetConnector() const { return m_connector; }
  const Rule& GetRule() const { return m_rule; }
  STree* GetParent() const { return m_parent; }
  const IList& IStart() const { return m_iconnection.Start(); }
  const IList& IEnd() const { return m_iconnection.End(); }

  IConnection& GetIConnection() { return m_iconnection; }
  OutputFunc& GetOutputFunc() const { return *m_outputFunc.get(); }

  operator std::string() const;
  std::string DrawNode(const std::string& context) const;

private:
  void Restart(const IList& istart);

  IConnection m_iconnection;
  std::auto_ptr<RestartFunc> m_restartFunc;
  std::auto_ptr<ComputeFunc> m_computeFunc;
  std::auto_ptr<OutputFunc> m_outputFunc;
};

std::ostream& operator<< (std::ostream& out, const STree& node);

}

#endif // _STree_h_
