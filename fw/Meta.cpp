// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#include "Meta.h"

#include "Connector.h"
#include "FWTree.h"

#include <memory>
#include <string>
using std::auto_ptr;
using std::string;

using namespace fw;

void MetaRule::Update(Connector& connector, FWTree& x) const {
  m_log.info("Meta: updating " + string(*this) + " at " + string(x));
  State& state = x.GetState();
  state.GoBad();
  const IList* first = x.iconnection.istart;
  if (first->name == m_searchName) {
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
