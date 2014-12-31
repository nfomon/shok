// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Rule_h_
#define _Rule_h_

/* Production rules
 *
 * A Rule represents a node of a parser-machine graph.  In essence it is a
 * factory for FWTree nodes which represent the process of the parse attempt.
 */

#include "FWError.h"
#include "IList.h"
#include "OutputFunc.h"
#include "RestartFunc.h"
#include "State.h"

#include <boost/lexical_cast.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <memory>
#include <string>

namespace fw {

class Connector;
class FWTree;

class Rule {
public:
  typedef boost::ptr_vector<Rule> child_vec;
  typedef child_vec::const_iterator child_iter;

  Rule(Log& log, const std::string& debugName, RestartFuncType rft, OutputFuncType ost)
    : m_log(log),
      m_name(debugName),
      m_restartFuncType(rft),
      m_outputFuncType(ost),
      m_parent(NULL) {}
  virtual ~Rule() {}

  const child_vec& GetChildren() const { return m_children; }

  std::auto_ptr<FWTree> MakeRootNode(Connector& connector) const;
  FWTree* MakeNode(FWTree& parent, const IList& istart) const;

  std::auto_ptr<RestartFunc> MakeRestartFunc(FWTree& x) const;
  std::auto_ptr<OutputFunc> MakeOutputFunc(const FWTree& x) const;

  // Reposition and Update have some responsibilities:
  //  1. Clean up / clear out state as appropriate
  //  2. Listen/Unlisten for Input updates as appropriate
  //  3. Compute new state flags based on children
  //  4. Update begin/end inodes
  // Reposition should RepositionNode() its children as necessary to then
  // calculate its own state.  Connector::UpdateNode() will be automatically
  // called by Connector::RepositionNode().
  //virtual void Reposition(Connector& connector, FWTree& x, const IList& inode) const {}

  // Calculate local state flags based on children's state, under the
  // assumption that the children are already up-to-date.
  // Returns true if the node was changed.
  virtual void Update(FWTree& x) const = 0;

  Rule* AddChild(std::auto_ptr<Rule> child) {
    child->setParent(this);
    m_children.push_back(child);
    return &m_children.back();
  }
  template <typename T>
  Rule* CreateChild(const std::string& name) {
    std::auto_ptr<Rule> child(new T(m_log, name));
    return AddChild(child);
  }
  template <typename T, typename A1>
  Rule* CreateChild(const std::string& name, const A1& a1) {
    std::auto_ptr<Rule> child(new T(m_log, name, a1));
    return AddChild(child);
  }

  std::string Name() const { return m_name; }

  virtual operator std::string() const {
    return m_name + ":" + boost::lexical_cast<std::string>(m_children.size());
  }
  std::string Print() const;
  virtual std::string DrawNode(const std::string& context) const;

protected:
  void setParent(Rule* parent) {
    m_parent = parent;
  }

  Log& m_log;
  std::string m_name;
  RestartFuncType m_restartFuncType;
  OutputFuncType m_outputFuncType;
  Rule* m_parent;
  child_vec m_children;
};

}

#endif // _Rule_h_
