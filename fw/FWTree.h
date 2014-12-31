// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _FWTree_h_
#define _FWTree_h_

#include "Connector.h"
#include "IConnection.h"
#include "FWError.h"
#include "Hotlist.h"
#include "OutputFunc.h"
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
  Log& m_log;
  Connector& m_connector;
  const Rule& m_rule;
  State m_state;
  FWTree* m_parent;

public:
  child_vec children;
  depth_t depth;

  FWTree(Log& log, Connector& connector, const Rule& rule, FWTree* parent);
  void Init(std::auto_ptr<RestartFunc> restartFunc,
            std::auto_ptr<OutputFunc> outputFunc);

  void RestartNode(const IList& istart);
  bool UpdateNode();
  void ClearNode();

  const State& GetState() const { return m_state; }
  State& GetState() { return m_state; }
  Connector& GetConnector() const { return m_connector; }
  const Rule& GetRule() const { return m_rule; }
  FWTree* GetParent() const { return m_parent; }
  const IList& IStart() const { return m_iconnection.Start(); }
  const IList& IEnd() const { return m_iconnection.End(); }

  IConnection& GetIConnection() { return m_iconnection; }
  OutputFunc& GetOutputFunc() const { return *m_outputFunc.get(); }

  operator std::string() const;
  std::string DrawNode(const std::string& context) const;

private:
  void Restart(const IList& istart);

  std::auto_ptr<RestartFunc> m_restartFunc;
  IConnection m_iconnection;
  std::auto_ptr<OutputFunc> m_outputFunc;
};

}

#endif // _FWTree_h_
