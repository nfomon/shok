// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Rule_h_
#define _Rule_h_

#include "DS.h"
#include "FWError.h"
#include "State.h"

#include <boost/lexical_cast.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <memory>

namespace fw {

class Connector;

class RuleState;

class Rule {
public:
  typedef boost::ptr_vector<Rule> child_vec;
  typedef child_vec::const_iterator child_iter;

  Rule(Log& log, const std::string& name = "")
    : m_log(log),
      m_name(name),
      m_parent(NULL) {}
  virtual ~Rule() {}

  virtual std::auto_ptr<State> MakeState() const;

  // Reposition and Update have some responsibilities:
  //  1. Clean up / clear out state as appropriate
  //  2. Listen/Unlisten for updates as appropriate (leaves only)
  //  3. Compute new state flags based on children
  //  4. Update size and begin/end inodes
  // Reposition should InsertNode() and/or RepositionNode() its children as
  // necessary to then calculate its own state.
  virtual void Reposition(Connector& connector, TreeDS& x, const IList& inode) const = 0;

  // Calculate local state flags based on children's state, under the
  // assumption that the children are already up-to-date.
  // Returns true if the node was changed
  virtual bool Update(Connector& connector, TreeDS& x, const TreeDS* child) const = 0;

  void AddChild(std::auto_ptr<Rule> child) {
    //m_log.debug("Rule: Adding child " + std::string(*child.get()) + " to " + std::string(*this));
    child->setParent(this);
    m_children.push_back(child);
  }

  virtual operator std::string() const { return "(" + m_name + ":" + boost::lexical_cast<std::string>(m_children.size()) + ")"; }
  virtual std::string print() const {
    std::string s(m_name);
    if (!m_children.empty()) {
      s += " (";
      for (child_iter i = m_children.begin(); i != m_children.end(); ++i) {
        if (i != m_children.begin()) { s += ", "; }
        s += i->print();
      }
      s += ")";
    }
    return s;
  }

protected:
  void setParent(Rule* parent) {
    m_parent = parent;
  }

  Log& m_log;
  std::string m_name;
  Rule* m_parent;
  child_vec m_children;
};

struct RuleState : public State {
  const Rule& rule;
  RuleState(const Rule& rule)
    : rule(rule) {}
  virtual ~RuleState() {}

  virtual operator std::string() const { return "[RuleState:" + StateFlags() + "]"; }
};

}

#endif // _Rule_h_
