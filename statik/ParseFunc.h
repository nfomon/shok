// Copyright (C) 2014 Michael Biggs.  See the COPYING file at the top-level
// directory of this distribution and at http://shok.io/code/copyright.html

#ifndef _statik_ParseFunc_h_
#define _statik_ParseFunc_h_

#include "List.h"
#include "ParseAction.h"

#include <memory>

namespace statik {

struct ParseFunc {
public:
  virtual ~ParseFunc() {}
  void Init(STree& x) { m_node = &x; }
  virtual void operator() (ParseAction::Action action, const List& inode, const STree* initiator) = 0;
  virtual std::auto_ptr<ParseFunc> Clone() = 0;
protected:
  STree* m_node;
};

}

#endif // _statik_ParseFunc_h_
