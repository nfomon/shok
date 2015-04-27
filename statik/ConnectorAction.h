// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _ConnectorAction_h_
#define _ConnectorAction_h_

#include "IList.h"

namespace statik {

class STree;

struct ConnectorAction {
public:
  enum Action {
    Start,
    Restart,
    INodeInsert,
    INodeDelete,
    INodeUpdate,
    ChildUpdate
  };

  static std::string UnMapAction(Action action) {
    switch (action) {
      case Start: return "Start";
      case Restart: return "Restart";
      case INodeInsert: return "INodeInsert";
      case INodeDelete: return "INodeDelete";
      case INodeUpdate: return "INodeUpdate";
      case ChildUpdate: return "ChildUpdate";
      default: throw SError("Cannot unmap unknown action");
    }
  }

  ConnectorAction(Action action, STree& node, const IList& inode, int resize = 0, const STree* initiator = NULL)
    : action(action),
      node(&node),
      inode(&inode),
      resize(resize),
      initiator(initiator) {}
  virtual ~ConnectorAction() {}

  Action action;
  STree* node;
  const IList* inode;
  int resize;
  const STree* initiator;
};

}

#endif // _ConnectorAction_h_
