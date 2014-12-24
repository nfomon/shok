// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Rule_h_
#define _Rule_h_

#include "FWError.h"
#include "OData.h"

#include <boost/lexical_cast.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <memory>

namespace fw {

class Connector;

class IList;
class OConnection;
class State;
class FWTree;

class Rule {
public:
  typedef boost::ptr_vector<Rule> child_vec;
  typedef child_vec::const_iterator child_iter;

  Rule(Log& log, const std::string& debugName)
    : m_log(log),
      m_name(debugName),
      m_parent(NULL) {}
  virtual ~Rule() {}

  virtual std::auto_ptr<OData> MakeData(const FWTree& x) const { return std::auto_ptr<OData>(); }
  virtual std::auto_ptr<OConnection> MakeOConnection(const FWTree& x) const = 0;

  // Reposition and Update have some responsibilities:
  //  1. Clean up / clear out state as appropriate
  //  2. Listen/Unlisten for updates as appropriate (leaves only)
  //  3. Compute new state flags based on children
  //  4. Update size and begin/end inodes
  // Reposition should InsertNode() and/or RepositionNode() its children as
  // necessary to then calculate its own state.
  virtual void Reposition(Connector& connector, FWTree& x, const IList& inode) const = 0;

  // Calculate local state flags based on children's state, under the
  // assumption that the children are already up-to-date.
  // Returns true if the node was changed
  virtual void Update(Connector& connector, FWTree& x) const = 0;

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

  // Convenience methods for some rules
  void AddChildToNode(FWTree& x, const Rule& child) const;
  void RepositionFirstChildOfNode(Connector& connector, FWTree& x, const IList& inode) const;
  void RepositionAllChildrenOfNode(Connector& connector, FWTree& x, const IList& inode) const;

  Log& m_log;
  std::string m_name;
  Rule* m_parent;
  child_vec m_children;
};

}

#endif // _Rule_h_
