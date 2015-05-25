// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_STree_h_
#define _statik_STree_h_

#include "ComputeFunc.h"
#include "IConnection.h"
#include "OutputFunc.h"
#include "ParseAction.h"
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
         std::auto_ptr<ComputeFunc> computeFunc,
         std::auto_ptr<OutputFunc> outputFunc);

  void StartNode(const IList& istart);
  void ComputeNode(ParseAction::Action action, const IList& inode, const STree* initiator);
  void ClearNode(const IList& inode);

  bool IsClear() const { return m_isClear; }
  const State& GetState() const { return m_state; }
  State& GetState() { return m_state; }
  Connector& GetConnector() const { return m_connector; }
  const Rule& GetRule() const { return m_rule; }
  STree* GetParent() const { return m_parent; }
  const IList& IStart() const { return m_iconnection.Start(); }
  const IList& IEnd() const { return m_iconnection.End(); }

  IConnection& GetIConnection() { return m_iconnection; }
  const IConnection& GetIConnection() const { return m_iconnection; }
  OutputFunc& GetOutputFunc() const { return *m_outputFunc.get(); }

  operator std::string() const;
  std::string DrawNode(const std::string& context, const STree* initiator = NULL) const;

private:
  IConnection m_iconnection;
  std::auto_ptr<ComputeFunc> m_computeFunc;
  std::auto_ptr<OutputFunc> m_outputFunc;

  void ClearSubNode();
};

std::ostream& operator<< (std::ostream& out, const STree& node);

}

#endif // _statik_STree_h_
