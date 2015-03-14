// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _ConnectorAction_h_
#define _ConnectorAction_h_

#include "IList.h"
#include "STree.h"

namespace statik {

struct ConnectorAction {
public:
  enum Action {
    Restart,
    INodeInsert,
    INodeDelete,
    INodeUpdate,
    ChildGrow,
    ChildShrink,
    ChildUpdate,
    ClearChild
  };

  ConnectorAction(Action action, STree& node, const IList& inode, const STree* initiator = NULL)
    : action(action),
      node(&node),
      inode(&inode),
      initiator(initiator) {}
  virtual ~ConnectorAction() {}

  Action action;
  STree* node;
  const IList* inode;
  const STree* initiator;
};

}

#endif // _ConnectorAction_h_
