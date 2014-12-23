// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _Meta_h_
#define _Meta_h_

/* Meta rule
 *
 * Recognizes a node output by another rule, by its name.
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

class MetaRule;

struct MetaState : public State {
  MetaState(const MetaRule& rule);
  virtual ~MetaState() {}

  const MetaRule& GetMetaRule() const;

  virtual operator std::string() const;
};

class MetaRule : public Rule {
public:
  MetaRule(Log& log, const std::string& searchName, const std::string& name = "")
    : Rule(log, (name.empty() ? searchName : name)),
      m_searchName(searchName) {}
  virtual ~MetaRule() {}

  const std::string& GetString() const { return m_searchName; }

  virtual void Reposition(Connector& connector, FWTree& x, const IList& inode) const {
    Update(connector, x);
  }

  virtual void Update(Connector& connector, FWTree& x) const {
    m_log.info("Meta: updating " + std::string(*this) + " at " + std::string(x));
    State& state = x.GetState();
    state.GoBad();
    const IList* first = x.iconnection.istart;
    if (!first->owner) {
      throw FWError("MetaRule " + std::string(*this) + " first INode has no owner");
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
    m_log.debug("Meta" + std::string(*this) + " now: " + std::string(x));
  }

  virtual std::auto_ptr<State> MakeState() const;
  virtual std::auto_ptr<OConnection> MakeOConnection(const FWTree& x) const;

private:
  std::string m_searchName;
};

MetaState::MetaState(const MetaRule& rule)
  : State(rule) {}

const MetaRule& MetaState::GetMetaRule() const { return *dynamic_cast<const MetaRule*>(&rule); }

MetaState::operator std::string() const { return "Meta " + rule.Name() + " (" + GetMetaRule().GetString() + "):" + Print(); }

std::auto_ptr<State> MetaRule::MakeState() const {
  return std::auto_ptr<State>(new MetaState(*this));
}

std::auto_ptr<OConnection> MetaRule::MakeOConnection(const FWTree& x) const {
  return std::auto_ptr<OConnection>(new OConnectionSingle(m_log, x));
}

}

#endif // _Meta_h_
