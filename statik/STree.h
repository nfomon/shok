// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _STree_h_
#define _STree_h_

#include "ComputeFunc.h"
#include "ConnectorAction.h"
#include "IConnection.h"
#include "OutputFunc.h"
#include "RestartFunc.h"
#include "State.h"

#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace statik {

class Connector;
class Rule;

class STree {
public:
  typedef unsigned int depth_t;
  typedef std::vector<STree*> child_vec;
  typedef child_vec::const_iterator child_iter;
  typedef child_vec::iterator child_mod_iter;

private:
  Connector& m_connector;
  const Rule& m_rule;
  State m_state;
  STree* m_parent;
  bool m_isClear;

public:
  child_vec children;
  depth_t depth;

  STree(Connector& connector,
         const Rule& rule, STree* parent,
         std::auto_ptr<RestartFunc> restartFunc,
         std::auto_ptr<ComputeFunc> computeFunc,
         std::auto_ptr<OutputFunc> outputFunc);

  void RestartNode(const IList& istart);
  void ComputeNode(ConnectorAction::Action action, const IList& inode, const STree* initiator);
  void ClearNode();

  bool IsClear() const { return m_isClear; }
  const State& GetState() const { return m_state; }
  State& GetState() { return m_state; }
  Connector& GetConnector() const { return m_connector; }
  const Rule& GetRule() const { return m_rule; }
  STree* GetParent() const { return m_parent; }
  const IList& IStart() const { return m_iconnection.Start(); }
  const IList& IEnd() const { return m_iconnection.End(); }
  size_t ISize() const { return m_iconnection.Size(); }
  bool ContainsINode(const IList& inode) const;

  IConnection& GetIConnection() { return m_iconnection; }
  OutputFunc& GetOutputFunc() const { return *m_outputFunc.get(); }

  operator std::string() const;
  std::string DrawNode(const std::string& context) const;

private:
  IConnection m_iconnection;
  std::auto_ptr<RestartFunc> m_restartFunc;
  std::auto_ptr<ComputeFunc> m_computeFunc;
  std::auto_ptr<OutputFunc> m_outputFunc;
};

std::ostream& operator<< (std::ostream& out, const STree& node);

}

#endif // _STree_h_
