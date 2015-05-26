// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_STree_h_
#define _statik_STree_h_

#include "IConnection.h"
#include "List.h"
#include "OutputFunc.h"
#include "ParseAction.h"
#include "ParseFunc.h"
#include "State.h"

#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace statik {

class IncParser;
class Rule;

class STree {
public:
  typedef unsigned int depth_t;
  typedef std::vector<STree*> child_vec;
  typedef child_vec::const_iterator child_iter;
  typedef child_vec::iterator child_mod_iter;

private:
  IncParser& m_incParser;
  const Rule& m_rule;
  State m_state;
  STree* m_parent;
  bool m_isClear;

public:
  child_vec children;
  depth_t depth;

  STree(IncParser& incParser,
         const Rule& rule, STree* parent,
         std::auto_ptr<ParseFunc> parseFunc,
         std::auto_ptr<OutputFunc> outputFunc);

  void StartNode(const List& istart);
  void ParseNode(ParseAction::Action action, const List& inode, const STree* initiator);
  void ClearNode(const List& inode);

  bool IsClear() const { return m_isClear; }
  const State& GetState() const { return m_state; }
  State& GetState() { return m_state; }
  IncParser& GetIncParser() const { return m_incParser; }
  const Rule& GetRule() const { return m_rule; }
  STree* GetParent() const { return m_parent; }
  const List& IStart() const { return m_iconnection.Start(); }
  const List& IEnd() const { return m_iconnection.End(); }

  IConnection& GetIConnection() { return m_iconnection; }
  const IConnection& GetIConnection() const { return m_iconnection; }
  OutputFunc& GetOutputFunc() const { return *m_outputFunc.get(); }

  operator std::string() const;
  std::string DrawNode(const std::string& context, const STree* initiator = NULL) const;

private:
  IConnection m_iconnection;
  std::auto_ptr<ParseFunc> m_parseFunc;
  std::auto_ptr<OutputFunc> m_outputFunc;

  void ClearSubNode();
};

std::ostream& operator<< (std::ostream& out, const STree& node);

}

#endif // _statik_STree_h_
