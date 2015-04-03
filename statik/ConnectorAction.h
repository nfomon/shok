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
    Restart,
    Compute,    // Determine leaf state after a restart
    INodeInsert,
    INodeDelete,
    INodeUpdate,
    ChildGrow,
    ChildShrink,
    ChildUpdate,
    ClearChild
  };

  static std::string UnMapAction(Action action) {
    switch (action) {
      case Restart: return "Restart";
      case Compute: return "Compute";
      case INodeInsert: return "INodeInsert";
      case INodeDelete: return "INodeDelete";
      case INodeUpdate: return "INodeUpdate";
      case ChildGrow: return "ChildGrow";
      case ChildShrink: return "ChildShrink";
      case ChildUpdate: return "ChildUpdate";
      case ClearChild: return "ClearChild";
      default: throw SError("Cannot unmap unknown action");
    }
  }

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
