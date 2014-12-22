// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Name_h_
#define _Name_h_

/* Name rule
 *
 * Recognizes a node by its name.
 */

#include "Connector.h"
#include "FWError.h"
#include "FWTree.h"
#include "OData.h"
#include "Rule.h"

#include <memory>
#include <string>
#include <utility>

namespace fw {

class NameRule;

struct NameState : public State {
  NameState(const NameRule& rule);
  virtual ~NameState() {}

  const NameRule& GetNameRule() const;

  virtual operator std::string() const;
};

class NameRule : public Rule {
public:
  NameRule(Log& log, const std::string& searchName, const std::string& name)
    : Rule(log, name),
      m_searchName(searchName) {}
  virtual ~NameRule() {}

  const std::string& GetString() const { return m_searchName; }

  virtual void Reposition(Connector& connector, FWTree& x, const IList& inode) const {
    Update(connector, x);
  }

  virtual void Update(Connector& connector, FWTree& x) const {
    m_log.info("Name: updating " + std::string(*this) + " at " + std::string(x));
    State& state = x.GetState();
    state.GoBad();
    const IList* first = x.iconnection.istart;
    if (!first->owner) {
      throw FWError("NameRule " + std::string(*this) + " first INode has no owner");
    }
    if (first->owner->GetState().rule.Name() == m_searchName) {
      connector.Listen(x, *first);
      state.GoDone();
      x.iconnection.size = 1;
      const IList* second = first->right;
      if (second) {
        state.GoComplete();
        x.iconnection.iend = second;
      } else {
        x.iconnection.iend = NULL;
      }
    }
    m_log.debug("Name" + std::string(*this) + " now: " + std::string(x));
  }

  virtual std::auto_ptr<State> MakeState() const;
  virtual std::auto_ptr<OConnection> MakeOConnection(const FWTree& x) const;

private:
  std::string m_searchName;
};

NameState::NameState(const NameRule& rule)
  : State(rule) {}

const NameRule& NameState::GetNameRule() const { return *dynamic_cast<const NameRule*>(&rule); }

NameState::operator std::string() const { return "Name " + rule.Name() + " (" + GetNameRule().GetString() + "):" + Print(); }

std::auto_ptr<State> NameRule::MakeState() const {
  return std::auto_ptr<State>(new NameState(*this));
}

std::auto_ptr<OConnection> NameRule::MakeOConnection(const FWTree& x) const {
  return std::auto_ptr<OConnection>(new OConnectionSingle(m_log, x));
}

}

#endif // _Name_h_
