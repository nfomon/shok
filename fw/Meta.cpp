// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Meta.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace fw;

void MetaRule::Reposition(Connector& connector, FWTree& x, const IList& inode) const {
  Update(connector, x);
}

void MetaRule::Update(Connector& connector, FWTree& x) const {
  m_log.info("Meta: updating " + string(*this) + " at " + string(x));
  State& state = x.GetState();
  state.GoBad();
  const IList* first = x.iconnection.istart;
  if (!first->owner) {
    throw FWError("MetaRule " + string(*this) + " first INode has no owner");
  }
  if (first->owner->GetRule().Name() == m_searchName) {
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
  m_log.debug("Meta" + string(*this) + " now: " + string(x));
}

auto_ptr<OConnection> MetaRule::MakeOConnection(const FWTree& x) const {
  return auto_ptr<OConnection>(new OConnectionSingle(m_log, x));
}
